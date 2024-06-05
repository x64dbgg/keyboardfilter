#include <ntddk.h>
#include <wdf.h>
#include <kbdmou.h>
#include <ntddkbd.h>


#define DEVICE_NAME L"\\Device\\KeyboardFilter"
#define SYMLINK_NAME L"\\DosDevices\\KeyboardFilter"


PDEVICE_OBJECT g_KeyboardDevice = NULL;
PDEVICE_OBJECT g_NextKeyboardDevice = NULL;


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
NTSTATUS KeyboardFilterDispatchRead(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS KeyboardFilterDispatchPassThrough(PDEVICE_OBJECT DeviceObject, PIRP Irp);
VOID KeyboardFilterUnload(PDRIVER_OBJECT DriverObject);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, KeyboardFilterUnload)
#endif


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNICODE_STRING uniNtNameString, uniDosNameString;
    NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_OBJECT deviceObject = NULL;

    UNREFERENCED_PARAMETER(RegistryPath);

    RtlInitUnicodeString(&uniNtNameString, DEVICE_NAME);
    RtlInitUnicodeString(&uniDosNameString, SYMLINK_NAME);

    status = IoCreateDevice(DriverObject, 0, &uniNtNameString, FILE_DEVICE_KEYBOARD, 0, FALSE, &deviceObject);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    g_KeyboardDevice = deviceObject;

    status = IoCreateSymbolicLink(&uniDosNameString, &uniNtNameString);

    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        return status;
    }

    g_NextKeyboardDevice = IoAttachDeviceToDeviceStack(deviceObject, IoGetAttachedDeviceReference(deviceObject));
    
    if (g_NextKeyboardDevice == NULL) {
        IoDeleteSymbolicLink(&uniDosNameString);
        IoDeleteDevice(deviceObject);
        return STATUS_NO_SUCH_DEVICE;
    }

    DriverObject->MajorFunction[IRP_MJ_READ] = KeyboardFilterDispatchRead;
    DriverObject->MajorFunction[IRP_MJ_PNP] = KeyboardFilterDispatchPassThrough;
    DriverObject->DriverUnload = KeyboardFilterUnload;

    return STATUS_SUCCESS;
}

NTSTATUS KeyboardFilterDispatchRead(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PKEYBOARD_INPUT_DATA keys = (PKEYBOARD_INPUT_DATA)Irp->AssociatedIrp.SystemBuffer;
    ULONG numKeys = irpStack->Parameters.Read.Length / sizeof(KEYBOARD_INPUT_DATA);

    // Process each key press
    for (ULONG i = 0; i < numKeys; i++) {
        if (keys[i].MakeCode == 0x1E) { // 'a' key
            keys[i].MakeCode = 0x30; // 'A' key
        }
        DbgPrint("KeyboardFilter: Key Pressed - MakeCode: 0x%x, Flags: 0x%x\n", keys[i].MakeCode, keys[i].Flags);
    }

    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(g_NextKeyboardDevice, Irp);
}

NTSTATUS KeyboardFilterDispatchPassThrough(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(g_NextKeyboardDevice, Irp);
}

VOID KeyboardFilterUnload(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING uniDosNameString;
    RtlInitUnicodeString(&uniDosNameString, SYMLINK_NAME);

    IoDetachDevice(g_NextKeyboardDevice);
    IoDeleteSymbolicLink(&uniDosNameString);
    IoDeleteDevice(g_KeyboardDevice);
}
