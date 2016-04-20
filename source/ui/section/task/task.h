#pragma once

#include <sys/syslimits.h>

#include "../../list.h"

typedef struct {
    char shortDescription[0x100];
    char longDescription[0x200];
    char publisher[0x100];
    u32 texture;
} smdh_info;

typedef struct {
    FS_MediaType mediaType;
    u64 titleId;
    char productCode[0x10];
    u16 version;
    u64 installedSize;
    bool twl;
    bool hasSmdh;
    smdh_info smdhInfo;
} title_info;

typedef struct {
    FS_MediaType mediaType;
    u64 titleId;
    u16 version;
} pending_title_info;

typedef struct {
    u64 ticketId;
} ticket_info;

typedef struct {
    FS_MediaType mediaType;
    u64 extSaveDataId;
    bool shared;
    bool hasSmdh;
    smdh_info smdhInfo;
} ext_save_data_info;

typedef struct {
    u32 systemSaveDataId;
} system_save_data_info;

typedef struct {
    u64 titleId;
    u16 version;
    u64 installedSize;
    bool hasSmdh;
    smdh_info smdhInfo;
} cia_info;

typedef struct {
    FS_Archive* archive;
    char name[NAME_MAX];
    char path[PATH_MAX];
    bool isDirectory;
    u64 size;

    bool containsCias;
    bool isCia;
    cia_info ciaInfo;

    bool containsTickets;
    bool isTicket;
    ticket_info ticketInfo;
} file_info;

#define R_FBI_CANCELLED MAKERESULT(RL_PERMANENT, RS_CANCELED, RM_APPLICATION, 1)
#define R_FBI_ERRNO MAKERESULT(RL_PERMANENT, RS_INTERNAL, RM_APPLICATION, 2)
#define R_FBI_WRONG_SYSTEM MAKERESULT(RL_PERMANENT, RS_NOTSUPPORTED, RM_APPLICATION, 3)

#define R_FBI_OUT_OF_MEMORY MAKERESULT(RL_FATAL, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_MEMORY)

typedef enum {
    DATAOP_COPY,
    DATAOP_DELETE
} DataOp;

typedef struct {
    void* data;

    DataOp op;

    // Copy
    bool copyEmpty;

    bool finished;
    bool premature;

    u32 processed;
    u32 total;

    u64 currProcessed;
    u64 currTotal;

    Result (*isSrcDirectory)(void* data, u32 index, bool* isDirectory);
    Result (*makeDstDirectory)(void* data, u32 index);

    Result (*openSrc)(void* data, u32 index, u32* handle);
    Result (*closeSrc)(void* data, u32 index, bool succeeded, u32 handle);

    Result (*getSrcSize)(void* data, u32 handle, u64* size);
    Result (*readSrc)(void* data, u32 handle, u32* bytesRead, void* buffer, u64 offset, u32 size);

    Result (*openDst)(void* data, u32 index, void* initialReadBlock, u32* handle);
    Result (*closeDst)(void* data, u32 index, bool succeeded, u32 handle);

    Result (*writeDst)(void* data, u32 handle, u32* bytesWritten, void* buffer, u64 offset, u32 size);

    // Delete
    Result (*delete)(void* data, u32 index);

    // Errors
    bool (*error)(void* data, u32 index, Result res);
} data_op_info;

bool task_is_quit_all();
void task_quit_all();

Handle task_populate_ext_save_data(list_item* items, u32* count, u32 max);
Handle task_populate_files(list_item* items, u32* count, u32 max, file_info* dir);
Handle task_populate_pending_titles(list_item* items, u32* count, u32 max);
Handle task_populate_system_save_data(list_item* items, u32* count, u32 max);
Handle task_populate_tickets(list_item* items, u32* count, u32 max);
Handle task_populate_titles(list_item* items, u32* count, u32 max);
Handle task_data_op(data_op_info* info);