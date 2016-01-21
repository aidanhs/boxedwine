/*
 * Copyright (C) the Wine project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_WINIOCTL_H
#define __WINE_WINIOCTL_H

#include "guiddef.h"

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    (DWORD)((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define DEVICE_TYPE DWORD

#define FILE_DEVICE_BEEP                0x00000001
#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define FILE_DEVICE_CONTROLLER          0x00000004
#define FILE_DEVICE_DATALINK            0x00000005
#define FILE_DEVICE_DFS                 0x00000006
#define FILE_DEVICE_DISK                0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define FILE_DEVICE_FILE_SYSTEM         0x00000009
#define FILE_DEVICE_INPORT_PORT         0x0000000a
#define FILE_DEVICE_KEYBOARD            0x0000000b
#define FILE_DEVICE_MAILSLOT            0x0000000c
#define FILE_DEVICE_MIDI_IN             0x0000000d
#define FILE_DEVICE_MIDI_OUT            0x0000000e
#define FILE_DEVICE_MOUSE               0x0000000f
#define FILE_DEVICE_MULTI_UNC_PROVIDER  0x00000010
#define FILE_DEVICE_NAMED_PIPE          0x00000011
#define FILE_DEVICE_NETWORK             0x00000012
#define FILE_DEVICE_NETWORK_BROWSER     0x00000013
#define FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define FILE_DEVICE_NULL                0x00000015
#define FILE_DEVICE_PARALLEL_PORT       0x00000016
#define FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define FILE_DEVICE_PRINTER             0x00000018
#define FILE_DEVICE_SCANNER             0x00000019
#define FILE_DEVICE_SERIAL_MOUSE_PORT   0x0000001a
#define FILE_DEVICE_SERIAL_PORT         0x0000001b
#define FILE_DEVICE_SCREEN              0x0000001c
#define FILE_DEVICE_SOUND               0x0000001d
#define FILE_DEVICE_STREAMS             0x0000001e
#define FILE_DEVICE_TAPE                0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define FILE_DEVICE_TRANSPORT           0x00000021
#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_DEVICE_VIDEO               0x00000023
#define FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define FILE_DEVICE_WAVE_IN             0x00000025
#define FILE_DEVICE_WAVE_OUT            0x00000026
#define FILE_DEVICE_8042_PORT           0x00000027
#define FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028
#define FILE_DEVICE_BATTERY             0x00000029
#define FILE_DEVICE_BUS_EXTENDER        0x0000002a
#define FILE_DEVICE_MODEM               0x0000002b
#define FILE_DEVICE_VDM                 0x0000002c
#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define FILE_DEVICE_SMB                 0x0000002e
#define FILE_DEVICE_KS                  0x0000002f
#define FILE_DEVICE_CHANGER             0x00000030
#define FILE_DEVICE_SMARTCARD           0x00000031
#define FILE_DEVICE_ACPI                0x00000032
#define FILE_DEVICE_DVD                 0x00000033
#define FILE_DEVICE_FULLSCREEN_VIDEO    0x00000034
#define FILE_DEVICE_DFS_FILE_SYSTEM     0x00000035
#define FILE_DEVICE_DFS_VOLUME          0x00000036
#define FILE_DEVICE_SERENUM             0x00000037
#define FILE_DEVICE_TERMSRV             0x00000038
#define FILE_DEVICE_KSEC                0x00000039

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS             0
#define FILE_READ_ACCESS                FILE_READ_DATA  /* file & pipe */
#define FILE_WRITE_ACCESS               FILE_WRITE_DATA /* file & pipe */

#define FSCTL_REQUEST_OPLOCK_LEVEL_1             CTL_CODE(FILE_DEVICE_FILE_SYSTEM,   0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_REQUEST_OPLOCK_LEVEL_2             CTL_CODE(FILE_DEVICE_FILE_SYSTEM,   1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_REQUEST_BATCH_OPLOCK               CTL_CODE(FILE_DEVICE_FILE_SYSTEM,   2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_OPLOCK_BREAK_ACKNOWLEDGE           CTL_CODE(FILE_DEVICE_FILE_SYSTEM,   3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_OPBATCH_ACK_CLOSE_PENDING          CTL_CODE(FILE_DEVICE_FILE_SYSTEM,   4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_OPLOCK_BREAK_NOTIFY                CTL_CODE(FILE_DEVICE_FILE_SYSTEM,   5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_LOCK_VOLUME                        CTL_CODE(FILE_DEVICE_FILE_SYSTEM,   6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_UNLOCK_VOLUME                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,   7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_DISMOUNT_VOLUME                    CTL_CODE(FILE_DEVICE_FILE_SYSTEM,   8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_IS_VOLUME_MOUNTED                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_IS_PATHNAME_VALID                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_MARK_VOLUME_DIRTY                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_QUERY_RETRIEVAL_POINTERS           CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  14, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_GET_COMPRESSION                    CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_SET_COMPRESSION                    CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  16, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_MARK_AS_SYSTEM_HIVE                CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  19, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_OPLOCK_BREAK_ACK_NO_2              CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  20, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_INVALIDATE_VOLUMES                 CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_QUERY_FAT_BPB                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_REQUEST_FILTER_OPLOCK              CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  23, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_FILESYSTEM_GET_STATISTICS          CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  24, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_GET_NTFS_VOLUME_DATA               CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  25, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_GET_NTFS_FILE_RECORD               CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  26, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_GET_VOLUME_BITMAP                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  27, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_GET_RETRIEVAL_POINTERS             CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  28, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_MOVE_FILE                          CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  29, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_IS_VOLUME_DIRTY                    CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  30, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_GET_HFS_INFORMATION                CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  31, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_ALLOW_EXTENDED_DASD_IO             CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  32, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_READ_PROPERTY_DATA                 CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  33, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_WRITE_PROPERTY_DATA                CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  34, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_FIND_FILES_BY_SID                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  35, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_DUMP_PROPERTY_DATA                 CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  37, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_SET_OBJECT_ID                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  38, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_GET_OBJECT_ID                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  39, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_DELETE_OBJECT_ID                   CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  40, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_SET_REPARSE_POINT                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  41, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_GET_REPARSE_POINT                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_DELETE_REPARSE_POINT               CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  43, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_ENUM_USN_DATA                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  44, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_SECURITY_ID_CHECK                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  45, METHOD_NEITHER,  FILE_READ_DATA)
#define FSCTL_READ_USN_JOURNAL                   CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  46, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_SET_OBJECT_ID_EXTENDED             CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  47, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_CREATE_OR_GET_OBJECT_ID            CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  48, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_SET_SPARSE                         CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  49, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_SET_ZERO_DATA                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  50, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_QUERY_ALLOCATED_RANGES             CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  51, METHOD_NEITHER,  FILE_READ_DATA)
#define FSCTL_ENABLE_UPGRADE                     CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  52, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_SET_ENCRYPTION                     CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  53, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_ENCRYPTION_FSCTL_IO                CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  54, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_WRITE_RAW_ENCRYPTED                CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  55, METHOD_NEITHER,  FILE_SPECIAL_ACCESS)
#define FSCTL_READ_RAW_ENCRYPTED                 CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  56, METHOD_NEITHER,  FILE_SPECIAL_ACCESS)
#define FSCTL_CREATE_USN_JOURNAL                 CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  57, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_READ_FILE_USN_DATA                 CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  58, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_WRITE_USN_CLOSE_RECORD             CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  59, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_EXTEND_VOLUME                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  60, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_QUERY_USN_JOURNAL                  CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  61, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_DELETE_USN_JOURNAL                 CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  62, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_MARK_HANDLE                        CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  63, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_SIS_COPYFILE                       CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  64, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_SIS_LINK_FILES                     CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  65, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_HSM_MSG                            CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  66, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_HSM_DATA                           CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  68, METHOD_NEITHER,  FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_RECALL_FILE                        CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  69, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_READ_FROM_PLEX                     CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  71, METHOD_OUT_DIRECT,FILE_READ_DATA)
#define FSCTL_FILE_PREFETCH                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  72, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_MAKE_MEDIA_COMPATIBLE              CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  76, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_SET_DEFECT_MANAGEMENT              CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  77, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_QUERY_SPARING_INFO                 CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  78, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_QUERY_ON_DISK_VOLUME_INFO          CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  79, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_SET_VOLUME_COMPRESSION_STATE       CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  80, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_TXFS_MODIFY_RM                     CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  81, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_TXFS_QUERY_RM_INFORMATION          CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  82, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_TXFS_ROLLFORWARD_REDO              CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  84, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_TXFS_ROLLFORWARD_UNDO              CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  85, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_TXFS_START_RM                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  86, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_TXFS_SHUTDOWN_RM                   CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  87, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_TXFS_READ_BACKUP_INFORMATION       CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  88, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_TXFS_WRITE_BACKUP_INFORMATION      CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  89, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_TXFS_CREATE_SECONDARY_RM           CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  90, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_TXFS_GET_METADATA_INFO             CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  91, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_TXFS_GET_TRANSACTED_VERSION        CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  92, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_TXFS_CREATE_MINIVERSION            CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  95, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_TXFS_TRANSACTION_ACTIVE            CTL_CODE(FILE_DEVICE_FILE_SYSTEM,  99, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_SET_ZERO_ON_DEALLOCATION           CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 101, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_SET_REPAIR                         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 102, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_GET_REPAIR                         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 103, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_WAIT_FOR_REPAIR                    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 104, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_INITIATE_REPAIR                    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 106, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_CSC_INTERNAL                       CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 107, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FSCTL_SHRINK_VOLUME                      CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 108, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_SET_SHORT_NAME_BEHAVIOR            CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 109, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_DFSR_SET_GHOST_HANDLE_STATE        CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 110, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_TXFS_LIST_TRANSACTION_LOCKED_FILES CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 120, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_TXFS_LIST_TRANSACTIONS             CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 121, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_QUERY_PAGEFILE_ENCRYPTION          CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 122, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define FSCTL_PIPE_ASSIGN_EVENT         CTL_CODE(FILE_DEVICE_NAMED_PIPE, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_DISCONNECT           CTL_CODE(FILE_DEVICE_NAMED_PIPE, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_LISTEN               CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_PEEK                 CTL_CODE(FILE_DEVICE_NAMED_PIPE, 3, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_PIPE_QUERY_EVENT          CTL_CODE(FILE_DEVICE_NAMED_PIPE, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_TRANSCEIVE           CTL_CODE(FILE_DEVICE_NAMED_PIPE, 5, METHOD_NEITHER,  FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_PIPE_WAIT                 CTL_CODE(FILE_DEVICE_NAMED_PIPE, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_IMPERSONATE          CTL_CODE(FILE_DEVICE_NAMED_PIPE, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_SET_CLIENT_PROCESS   CTL_CODE(FILE_DEVICE_NAMED_PIPE, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_QUERY_CLIENT_PROCESS CTL_CODE(FILE_DEVICE_NAMED_PIPE, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_INTERNAL_READ        CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2045, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_PIPE_INTERNAL_WRITE       CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2046, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_PIPE_INTERNAL_TRANSCEIVE  CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2047, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_PIPE_INTERNAL_READ_OVFLOW CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2048, METHOD_BUFFERED, FILE_READ_DATA)

#define IOCTL_STORAGE_BASE FILE_DEVICE_MASS_STORAGE
#define IOCTL_STORAGE_CHECK_VERIFY       CTL_CODE(IOCTL_STORAGE_BASE, 0x0200, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_CHECK_VERIFY2      CTL_CODE(IOCTL_STORAGE_BASE, 0x0200, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORAGE_MEDIA_REMOVAL      CTL_CODE(IOCTL_STORAGE_BASE, 0x0201, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_EJECT_MEDIA        CTL_CODE(IOCTL_STORAGE_BASE, 0x0202, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_LOAD_MEDIA         CTL_CODE(IOCTL_STORAGE_BASE, 0x0203, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_LOAD_MEDIA2        CTL_CODE(IOCTL_STORAGE_BASE, 0x0203, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORAGE_RESERVE            CTL_CODE(IOCTL_STORAGE_BASE, 0x0204, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_RELEASE            CTL_CODE(IOCTL_STORAGE_BASE, 0x0205, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_FIND_NEW_DEVICES   CTL_CODE(IOCTL_STORAGE_BASE, 0x0206, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_EJECTION_CONTROL   CTL_CODE(IOCTL_STORAGE_BASE, 0x0250, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORAGE_MCN_CONTROL        CTL_CODE(IOCTL_STORAGE_BASE, 0x0251, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_STORAGE_GET_MEDIA_TYPES         CTL_CODE(IOCTL_STORAGE_BASE, 0x0300, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORAGE_GET_MEDIA_TYPES_EX      CTL_CODE(IOCTL_STORAGE_BASE, 0x0301, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER CTL_CODE(IOCTL_STORAGE_BASE, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORAGE_GET_HOTPLUG_INFO        CTL_CODE(IOCTL_STORAGE_BASE, 0x0305, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORAGE_SET_HOTPLUG_INFO        CTL_CODE(IOCTL_STORAGE_BASE, 0x0306, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_STORAGE_RESET_BUS          CTL_CODE(IOCTL_STORAGE_BASE, 0x0400, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_RESET_DEVICE       CTL_CODE(IOCTL_STORAGE_BASE, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_BREAK_RESERVATION  CTL_CODE(IOCTL_STORAGE_BASE, 0x0405, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_PERSISTENT_RESERVE_IN   CTL_CODE(IOCTL_STORAGE_BASE, 0x0406, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_STORAGE_PERSISTENT_RESERVE_OUT  CTL_CODE(IOCTL_STORAGE_BASE, 0x0407, METHOD_BUFFERED, FILE_READ_ACCESS|FILE_WRITE_ACCESS)

#define IOCTL_STORAGE_GET_DEVICE_NUMBER  CTL_CODE(IOCTL_STORAGE_BASE, 0x0420, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_DISK_BASE                 FILE_DEVICE_DISK
#define IOCTL_DISK_GET_DRIVE_GEOMETRY   CTL_CODE(IOCTL_DISK_BASE, 0x0000, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_GET_PARTITION_INFO   CTL_CODE(IOCTL_DISK_BASE, 0x0001, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DISK_SET_PARTITION_INFO   CTL_CODE(IOCTL_DISK_BASE, 0x0002, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_DISK_GET_DRIVE_LAYOUT     CTL_CODE(IOCTL_DISK_BASE, 0x0003, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DISK_SET_DRIVE_LAYOUT     CTL_CODE(IOCTL_DISK_BASE, 0x0004, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_DISK_VERIFY               CTL_CODE(IOCTL_DISK_BASE, 0x0005, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_FORMAT_TRACKS        CTL_CODE(IOCTL_DISK_BASE, 0x0006, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_DISK_REASSIGN_BLOCKS      CTL_CODE(IOCTL_DISK_BASE, 0x0007, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_DISK_PERFORMANCE          CTL_CODE(IOCTL_DISK_BASE, 0x0008, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_IS_WRITABLE          CTL_CODE(IOCTL_DISK_BASE, 0x0009, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_LOGGING              CTL_CODE(IOCTL_DISK_BASE, 0x000a, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_FORMAT_TRACKS_EX     CTL_CODE(IOCTL_DISK_BASE, 0x000b, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_DISK_HISTOGRAM_STRUCTURE  CTL_CODE(IOCTL_DISK_BASE, 0x000c, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_HISTOGRAM_DATA       CTL_CODE(IOCTL_DISK_BASE, 0x000d, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_HISTOGRAM_RESET      CTL_CODE(IOCTL_DISK_BASE, 0x000e, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_REQUEST_STRUCTURE    CTL_CODE(IOCTL_DISK_BASE, 0x000f, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_REQUEST_DATA         CTL_CODE(IOCTL_DISK_BASE, 0x0010, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DISK_CHECK_VERIFY         CTL_CODE(IOCTL_DISK_BASE, 0x0200, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DISK_MEDIA_REMOVAL        CTL_CODE(IOCTL_DISK_BASE, 0x0201, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DISK_EJECT_MEDIA          CTL_CODE(IOCTL_DISK_BASE, 0x0202, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DISK_LOAD_MEDIA           CTL_CODE(IOCTL_DISK_BASE, 0x0203, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DISK_RESERVE              CTL_CODE(IOCTL_DISK_BASE, 0x0204, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DISK_RELEASE              CTL_CODE(IOCTL_DISK_BASE, 0x0205, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DISK_FIND_NEW_DEVICES     CTL_CODE(IOCTL_DISK_BASE, 0x0206, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DISK_GET_MEDIA_TYPES      CTL_CODE(IOCTL_DISK_BASE, 0x0300, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define IOCTL_DISK_CONTROLLER_NUMBER    CTL_CODE(IOCTL_DISK_BASE, 0x0011, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SMART_GET_VERSION               CTL_CODE(IOCTL_DISK_BASE, 0x0020, METHOD_BUFFERED, FILE_READ_ACCESS)
#define SMART_SEND_DRIVE_COMMAND        CTL_CODE(IOCTL_DISK_BASE, 0x0021, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define SMART_RCV_DRIVE_DATA            CTL_CODE(IOCTL_DISK_BASE, 0x0022, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_DISK_GET_DRIVE_GEOMETRY_EX CTL_CODE(IOCTL_DISK_BASE, 0x0028, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VOLUME_BASE ((DWORD)'V')
#define IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS      CTL_CODE(IOCTL_VOLUME_BASE, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SERIAL_LSRMST_INSERT      CTL_CODE(FILE_DEVICE_SERIAL_PORT,31,METHOD_BUFFERED,FILE_ANY_ACCESS)


/* Return Codes... should these be the same as something somewhere? */
#define PARTITION_ENTRY_UNUSED          0x00      /* Entry unused */
#define PARTITION_FAT_12                0x01      /* 12-bit FAT entries */
#define PARTITION_XENIX_1               0x02      /* Xenix */
#define PARTITION_XENIX_2               0x03      /* Xenix */
#define PARTITION_FAT_16                0x04      /* 16-bit FAT entries */
#define PARTITION_EXTENDED              0x05      /* Extended partition entry */
#define PARTITION_HUGE                  0x06      /* Huge partition MS-DOS V4 */
#define PARTITION_IFS                   0x07      /* IFS Partition */
#define PARTITION_FAT32                 0x0B      /* FAT32 */
#define PARTITION_FAT32_XINT13          0x0C      /* FAT32 using extended int13 services */
#define PARTITION_XINT13                0x0E      /* Win95 partition using extended int13 services */
#define PARTITION_XINT13_EXTENDED       0x0F      /* Same as type 5 but uses extended int13 services */
#define PARTITION_PREP                  0x41      /* PowerPC Reference Platform (PReP) Boot Partition */
#define PARTITION_LDM                   0x42      /* Logical Disk Manager partition */
#define PARTITION_UNIX                  0x63      /* Unix */

typedef enum _MEDIA_TYPE {
    Unknown, F5_1Pt2_512, F3_1Pt44_512, F3_2Pt88_512, F3_20Pt8_512, F3_720_512, F5_360_512,
    F5_320_512, F5_320_1024, F5_180_512, F5_160_512, RemovableMedia, FixedMedia, F3_120M_512,
    F3_640_512, F5_640_512, F5_720_512, F3_1Pt2_512, F3_1Pt23_1024, F5_1Pt23_1024, F3_128Mb_512,
    F3_230Mb_512, F8_256_128
} MEDIA_TYPE, *PMEDIA_TYPE;

typedef struct _FORMAT_PARAMETERS {
   MEDIA_TYPE           MediaType;
   DWORD                StartCylinderNumber;
   DWORD                EndCylinderNumber;
   DWORD                StartHeadNumber;
   DWORD                EndHeadNumber;
} FORMAT_PARAMETERS, *PFORMAT_PARAMETERS;

typedef WORD   BAD_TRACK_NUMBER;
typedef WORD   *PBAD_TRACK_NUMBER;

typedef struct _FORMAT_EX_PARAMETERS {
   MEDIA_TYPE           MediaType;
   DWORD                StartCylinderNumber;
   DWORD                EndCylinderNumber;
   DWORD                StartHeadNumber;
   DWORD                EndHeadNumber;
   WORD                 FormatGapLength;
   WORD                 SectorsPerTrack;
   WORD                 SectorNumber[1];
} FORMAT_EX_PARAMETERS, *PFORMAT_EX_PARAMETERS;

typedef struct _DISK_GEOMETRY {
    LARGE_INTEGER       Cylinders;
    MEDIA_TYPE          MediaType;
    DWORD               TracksPerCylinder;
    DWORD               SectorsPerTrack;
    DWORD               BytesPerSector;
} DISK_GEOMETRY, *PDISK_GEOMETRY;

typedef struct _DISK_GEOMETRY_EX {
    DISK_GEOMETRY       Geometry;
    LARGE_INTEGER       DiskSize;
    BYTE                Data[1];
} DISK_GEOMETRY_EX, *PDISK_GEOMETRY_EX;

typedef struct _PARTITION_INFORMATION {
    LARGE_INTEGER       StartingOffset;
    LARGE_INTEGER       PartitionLength;
    DWORD               HiddenSectors;
    DWORD               PartitionNumber;
    BYTE                PartitionType;
    BOOLEAN             BootIndicator;
    BOOLEAN             RecognizedPartition;
    BOOLEAN             RewritePartition;
} PARTITION_INFORMATION, *PPARTITION_INFORMATION;

typedef struct _SET_PARTITION_INFORMATION {
    BYTE                PartitionType;
} SET_PARTITION_INFORMATION, *PSET_PARTITION_INFORMATION;

typedef struct _DRIVE_LAYOUT_INFORMATION {
    DWORD               PartitionCount;
    DWORD               Signature;
    PARTITION_INFORMATION PartitionEntry[1];
} DRIVE_LAYOUT_INFORMATION, *PDRIVE_LAYOUT_INFORMATION;

typedef struct _VERIFY_INFORMATION {
    LARGE_INTEGER       StartingOffset;
    DWORD               Length;
} VERIFY_INFORMATION, *PVERIFY_INFORMATION;

typedef struct _REASSIGN_BLOCKS {
    WORD                Reserved;
    WORD                Count;
    DWORD               BlockNumber[1];
} REASSIGN_BLOCKS, *PREASSIGN_BLOCKS;

/* Start: _WIN32_WINNT >= 0x0400 */

typedef struct _DISK_CONTROLLER_NUMBER {
    DWORD               ControllerNumber;
    DWORD               DiskNumber;
} DISK_CONTROLLER_NUMBER, *PDISK_CONTROLLER_NUMBER;

/* VOLUME_GET_VOLUME_DISK_EXTENTS */
typedef struct _DISK_EXTENT {
    DWORD               DiskNumber;
    LARGE_INTEGER       StartingOffset;
    LARGE_INTEGER       ExtentLength;
} DISK_EXTENT;

typedef struct _VOLUME_DISK_EXTENTS {
    DWORD               NumberOfDiskExtents;
    DISK_EXTENT         Extents[1];
} VOLUME_DISK_EXTENTS;

typedef struct RETRIEVAL_POINTERS_BUFFER {
    DWORD         ExtentCount;
    LARGE_INTEGER StartingVcn;
    struct {
        LARGE_INTEGER NextVcn;
        LARGE_INTEGER Lcn;
    } Extents[1];
} RETRIEVAL_POINTERS_BUFFER, *PRETRIEVAL_POINTERS_BUFFER;

/* End: _WIN32_WINNT >= 0x0400 */

/*
 *	NT I/O-Manager
 */

/*
 * structures for NtQueryVolumeInformationFile
 * (wdm.h)
 */

/* FileFsVolumeInformation = 1 */
typedef struct _FILE_FS_VOLUME_INFORMATION {
	LARGE_INTEGER	VolumeCreationTime;
	ULONG		VolumeSerialNumber;
	ULONG		VolumeLabelLength;
	BOOLEAN		SupportsObjects;
	WCHAR		VolumeLabel[1];
} FILE_FS_VOLUME_INFORMATION, *PFILE_FS_VOLUME_INFORMATION;

/* FileFsLabelInformation = 2 */
/*
 unknown
*/

/* FileFsSizeInformation = 3 */
typedef struct _FILE_FS_SIZE_INFORMATION {
	LARGE_INTEGER	TotalAllocationUnits;
	LARGE_INTEGER	AvailableAllocationUnits;
	ULONG		SectorsPerAllocationUnit;
	ULONG		BytesPerSector;
} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

/* FileFsDeviceInformation = 4 */
typedef struct _FILE_FS_DEVICE_INFORMATION {
	DEVICE_TYPE DeviceType;
	ULONG Characteristics;
} FILE_FS_DEVICE_INFORMATION, *PFILE_FS_DEVICE_INFORMATION;

/* FileFsAttributeInformation = 5 */
typedef struct _FILE_FS_ATTRIBUTE_INFORMATION {
	ULONG	FileSystemAttribute;
	LONG	MaximumComponentNameLength;
	ULONG	FileSystemNameLength;
	WCHAR	FileSystemName[1];
} FILE_FS_ATTRIBUTE_INFORMATION, *PFILE_FS_ATTRIBUTE_INFORMATION;

typedef struct _FILE_PIPE_WAIT_FOR_BUFFER {
    LARGE_INTEGER   Timeout;
    ULONG           NameLength;
    BOOLEAN         TimeoutSpecified;
    WCHAR           Name[1];
} FILE_PIPE_WAIT_FOR_BUFFER, *PFILE_PIPE_WAIT_FOR_BUFFER;

typedef struct _FILE_PIPE_PEEK_BUFFER {
    ULONG   NamedPipeState;
    ULONG   ReadDataAvailable;
    ULONG   NumberOfMessages;
    ULONG   MessageLength;
    CHAR    Data[1];
} FILE_PIPE_PEEK_BUFFER, *PFILE_PIPE_PEEK_BUFFER;

/* Device GUIDs */
#ifdef DEFINE_GUID

DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
DEFINE_GUID(GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR, 0x4D36E978, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);

/* obsolete GUID names */
#define GUID_CLASS_COMPORT          GUID_DEVINTERFACE_COMPORT
#define GUID_SERENUM_BUS_ENUMERATOR GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR

#endif /* DEFINE_GUID */

#endif
