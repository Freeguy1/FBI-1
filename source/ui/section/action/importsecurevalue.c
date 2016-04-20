#include <stdio.h>

#include <3ds.h>

#include "action.h"
#include "../../error.h"
#include "../../info.h"
#include "../../prompt.h"
#include "../../../screen.h"
#include "../../../util.h"

static void action_import_secure_value_update(ui_view* view, void* data, float* progress, char* text) {
    title_info* info = (title_info*) data;

    char pathBuf[64];
    snprintf(pathBuf, 64, "/fbi/securevalue/%016llX.dat", info->titleId);

    Result res = 0;

    FS_Path* fsPath = util_make_path_utf8(pathBuf);

    FS_Archive sdmcArchive = {ARCHIVE_SDMC, {PATH_BINARY, 0, (void*) ""}};
    Handle fileHandle = 0;
    if(R_SUCCEEDED(res = FSUSER_OpenFileDirectly(&fileHandle, sdmcArchive, *fsPath, FS_OPEN_READ, 0))) {
        u32 bytesRead = 0;
        u64 value = 0;
        if(R_SUCCEEDED(res = FSFILE_Read(fileHandle, &bytesRead, 0, &value, sizeof(u64)))) {
            res = FSUSER_SetSaveDataSecureValue(value, SECUREVALUE_SLOT_SD, (u32) ((info->titleId >> 8) & 0xFFFFF), (u8) (info->titleId & 0xFF));
        }

        FSFILE_Close(fileHandle);
    }

    util_free_path_utf8(fsPath);

    ui_pop();
    info_destroy(view);

    if(R_SUCCEEDED(res)) {
        prompt_display("Success", "Secure value imported.", COLOR_TEXT, false, info, NULL, ui_draw_title_info, NULL);
    } else {
        error_display_res(NULL, info, ui_draw_title_info, res, "Failed to import secure value.");
    }
}

static void action_import_secure_value_onresponse(ui_view* view, void* data, bool response) {
    if(response) {
        info_display("Importing Secure Value", "", false, data, action_import_secure_value_update, ui_draw_title_info);
    }
}

void action_import_secure_value(title_info* info, bool* populated) {
    prompt_display("Confirmation", "Import the secure value of the selected title?", COLOR_TEXT, true, info, NULL, ui_draw_title_info, action_import_secure_value_onresponse);
}