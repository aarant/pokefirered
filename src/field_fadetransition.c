#include "global.h"
#include "palette.h"
#include "field_fadetransition.h"
#include "overworld.h"
#include "fldeff.h"
#include "field_weather.h"
#include "map_preview_screen.h"
#include "field_player_avatar.h"
#include "task.h"
#include "script.h"
#include "cable_club.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "quest_log.h"
#include "link.h"
#include "event_object_movement.h"
#include "sound.h"
#include "field_door.h"
#include "field_effect.h"
#include "field_screen_effect.h"
#include "field_specials.h"
#include "event_object_lock.h"
#include "start_menu.h"
#include "constants/songs.h"
#include "constants/event_object_movement.h"
#include "constants/field_weather.h"

static void sub_807DF4C(u8 a0);
static void sub_807DFBC(u8 taskId);
static void task_map_chg_seq_0807E20C(u8 taskId);
static void task_map_chg_seq_0807E2CC(u8 taskId);
static void Task_TeleportWarpIn(u8 taskId);
static void sub_807E718(u8 taskId);
static void Task_TeleportWarp(u8 taskId);
static void sub_807E80C(u8 taskId);
static void sub_807E980(u8 taskId);
static void sub_807EB64(u16, s16*, s16*);
static void sub_807EBBC(u8 a0, s16 *a1, s16 *a2);
static void sub_807EAC4(s16, s16, s16*, s16*, s16*);
static void sub_807EC34(u8 taskId);
static void sub_807ECBC(s16 *, s16 *, s16 *, s16 *, s16 *);
static bool8 sub_807EDA0(s16 *, s16 *, s16 *, s16 *, s16 *);

void palette_bg_faded_fill_white(void)
{
    CpuFastFill16(RGB_WHITE, gPlttBufferFaded, 0x400);
}

void palette_bg_faded_fill_black(void)
{
    CpuFastFill16(RGB_BLACK, gPlttBufferFaded, 0x400);
}

void WarpFadeInScreen(void)
{
    switch (MapTransitionIsExit(GetLastUsedWarpMapType(), GetCurrentMapType()))
    {
    case 0:
        palette_bg_faded_fill_black();
        FadeScreen(FADE_FROM_BLACK, 0);
        palette_bg_faded_fill_black();
        break;
    case 1:
        palette_bg_faded_fill_white();
        FadeScreen(FADE_FROM_WHITE, 0);
        palette_bg_faded_fill_white();
        break;
    }
}

static void sub_807DBAC(void)
{
    switch (MapTransitionIsExit(GetLastUsedWarpMapType(), GetCurrentMapType()))
    {
    case 0:
        palette_bg_faded_fill_black();
        FadeScreen(FADE_FROM_BLACK, 3);
        palette_bg_faded_fill_black();
        break;
    case 1:
        palette_bg_faded_fill_white();
        FadeScreen(FADE_FROM_WHITE, 3);
        palette_bg_faded_fill_white();
        break;
    }
}

void FadeInFromBlack(void)
{
    palette_bg_faded_fill_black();
    FadeScreen(FADE_FROM_BLACK, 0);
    palette_bg_faded_fill_black();
}

void WarpFadeOutScreen(void)
{
    const struct MapHeader *header = GetDestinationWarpMapHeader();
    if (header->regionMapSectionId != gMapHeader.regionMapSectionId && MapHasPreviewScreen(header->regionMapSectionId, MPS_TYPE_CAVE))
        FadeScreen(FADE_TO_BLACK, 0);
    else
    {
        switch (MapTransitionIsEnter(GetCurrentMapType(), header->mapType))
        {
        case FALSE:
            FadeScreen(FADE_TO_BLACK, 0);
            break;
        case TRUE:
            FadeScreen(FADE_TO_WHITE, 0);
            break;
        }
    }
}

static void sub_807DC70(void)
{
    switch (MapTransitionIsEnter(GetCurrentMapType(), GetDestinationWarpMapHeader()->mapType))
    {
    case FALSE:
        FadeScreen(FADE_TO_BLACK, 3);
        break;
    case TRUE:
        FadeScreen(FADE_TO_WHITE, 3);
        break;
    }
}

static void sub_807DCB0(bool8 arg)
{
    SetPlayerInvisibility(!arg);
}

static void task0A_nop_for_a_while(u8 taskId)
{
    if (FieldFadeTransitionBackgroundEffectIsFinished() == TRUE)
        DestroyTask(taskId);
}

void sub_807DCE4(void)
{
    ScriptContext2_Enable();
    Overworld_PlaySpecialMapMusic();
    FadeInFromBlack();
    CreateTask(task0A_nop_for_a_while, 10);
}

static void task0A_asap_script_env_2_enable_and_set_ctx_running(u8 taskId)
{
    if (FieldFadeTransitionBackgroundEffectIsFinished() == TRUE)
    {
        DestroyTask(taskId);
        EnableBothScriptContexts();
    }
}

void FieldCallback_ReturnToEventScript2(void)
{
    ScriptContext2_Enable();
    Overworld_PlaySpecialMapMusic();
    FadeInFromBlack();
    CreateTask(task0A_asap_script_env_2_enable_and_set_ctx_running, 10);
}

void sub_807DD44(void)
{
    ScriptContext2_Enable();
    FadeInFromBlack();
    CreateTask(task0A_asap_script_env_2_enable_and_set_ctx_running, 10);
}

static void task_mpl_807DD60(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[0])
    {
    case 0:
        task->data[1] = CreateTask_ReestablishLinkInCableClubRoom();
        task->data[0]++;
        break;
    case 1:
        if (gTasks[task->data[1]].isActive != TRUE)
        {
            WarpFadeInScreen();
            task->data[0]++;
        }
        break;
    case 2:
        if (FieldFadeTransitionBackgroundEffectIsFinished() == TRUE)
        {
            ScriptContext2_Disable();
            DestroyTask(taskId);
        }
        break;
    }
}

void FieldCB_ReturnToFieldWiredLink(void)
{
    ScriptContext2_Enable();
    Overworld_PlaySpecialMapMusic();
    palette_bg_faded_fill_black();
    CreateTask(task_mpl_807DD60, 10);
}

static void sub_807DDF0(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[0])
    {
    case 0:
        PrepareSendLinkCmd2FFE_or_RfuCmd6600();
        task->data[0]++;
        break;
    case 1:
        if (IsLinkTaskFinished())
        {
            WarpFadeInScreen();
            task->data[0]++;
        }
        break;
    case 2:
        if (FieldFadeTransitionBackgroundEffectIsFinished() == TRUE)
        {
            sub_8009FE8();
            ScriptContext2_Disable();
            DestroyTask(taskId);
        }
        break;
    }
}

void FieldCB_ReturnToFieldWirelessLink(void)
{
    ScriptContext2_Enable();
    Overworld_PlaySpecialMapMusic();
    palette_bg_faded_fill_black();
    CreateTask(sub_807DDF0, 10);
}

static void sub_807DE78(bool8 a0)
{
    s16 x, y;
    u32 behavior;
    TaskFunc func;

    PlayerGetDestCoords(&x, &y);
    behavior = MapGridGetMetatileBehaviorAt(x, y);
    if (MetatileBehavior_IsWarpDoor_2(behavior) == TRUE)
    {
        func = sub_807DFBC;
        switch (MapTransitionIsExit(GetLastUsedWarpMapType(), GetCurrentMapType()))
        {
        case 0:
            palette_bg_faded_fill_black();
            break;
        case 1:
            palette_bg_faded_fill_white();
            break;
        }
    }
    else
    {
        sub_807DF4C(a0);
        if (MetatileBehavior_IsNonAnimDoor(behavior) == TRUE)
            func = task_map_chg_seq_0807E20C;
        else if (MetatileBehavior_IsUnknownWarp6C_to_6F(behavior) == TRUE)
        {
            u8 tmp = gUnknown_2031DE0;
            func = task_map_chg_seq_0807E2CC;
            if (!tmp)
                func = sub_807EC34;
        }
        else
            func = task_map_chg_seq_0807E2CC;
    }
    gUnknown_2031DE0 = FALSE;
    CreateTask(func, 10);
}

static void sub_807DF4C(bool8 a0)
{
    if (!a0)
        WarpFadeInScreen();
    else
        FadeInFromBlack();
}

void FieldCB_UnionRoomWarp(void)
{
    Overworld_PlaySpecialMapMusic();
    sub_8111CF0();
    sub_807DE78(FALSE);
    ScriptContext2_Enable();
}

void sub_807DF7C(void)
{
    Overworld_PlaySpecialMapMusic();
    sub_8111CF0();
    sub_807DE78(TRUE);
    ScriptContext2_Enable();
}

static void FieldCB_TeleportWarpIn(void)
{
    Overworld_PlaySpecialMapMusic();
    WarpFadeInScreen();
    sub_8111CF0();
    PlaySE(SE_TK_WARPOUT);
    CreateTask(Task_TeleportWarpIn, 10);
    ScriptContext2_Enable();
}

static void sub_807DFBC(u8 taskId)
{
    struct Task * task = &gTasks[taskId];
    s16 *x = &task->data[2];
    s16 *y = &task->data[3];

    if (task->data[0] == 0)
        task->data[0] = 5;

    switch (task->data[0])
    {
    case 0: // Never reached
        sub_807DCB0(0);
        FreezeObjectEvents();
        PlayerGetDestCoords(x, y);
        FieldSetDoorOpened(*x, *y);
        task->data[0] = 1;
        break;
    case 5:
        sub_807DCB0(0);
        FreezeObjectEvents();
        DoOutwardBarnDoorWipe();
        sub_807DBAC();
        task->data[0] = 6;
        break;
    case 6:
        task->data[15]++;
        if (task->data[15] == 25)
        {
            PlayerGetDestCoords(x, y);
            PlaySE(GetDoorSoundEffect(*x, *y));
            FieldAnimateDoorOpen(*x, *y);
            task->data[0] = 7;
        }
        break;
    case 7:
        if (!FieldIsDoorAnimationRunning())
        {
            PlayerGetDestCoords(&task->data[12], &task->data[13]);
            sub_807DCB0(TRUE);
            ObjectEventSetHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(0xFF, 0, 0)], MOVEMENT_ACTION_WALK_NORMAL_DOWN);
            task->data[0] = 8;
        }
        break;
    case 8:
        task->data[14]++;
        if (task->data[14] == 14)
        {
            FieldAnimateDoorClose(task->data[12], task->data[13]);
            task->data[0] = 9;
        }
        break;
    case 9:
        if (FieldFadeTransitionBackgroundEffectIsFinished() && walkrun_is_standing_still() && !FieldIsDoorAnimationRunning() && !FuncIsActiveTask(Task_BarnDoorWipe))
        {
            ObjectEventClearHeldMovementIfFinished(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(0xFF, 0, 0)]);
            task->data[0] = 4;
        }
        break;
    // Legacy RS
    case 1:
        if (FieldFadeTransitionBackgroundEffectIsFinished())
        {
            sub_807DCB0(TRUE);
            ObjectEventSetHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(0xFF, 0, 0)], MOVEMENT_ACTION_WALK_NORMAL_DOWN);
            task->data[0] = 2;
        }
        break;
    case 2:
        if (walkrun_is_standing_still())
        {
            task->data[1] = FieldAnimateDoorClose(*x, *y);
            ObjectEventClearHeldMovementIfFinished(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(0xFF, 0, 0)]);
            task->data[0] = 3;
        }
        break;
    case 3:
        if (task->data[1] < 0 || gTasks[task->data[1]].isActive != TRUE)
            task->data[0] = 4;
        break;
    case 4:
        UnfreezeObjectEvents();
        ScriptContext2_Disable();
        DestroyTask(taskId);
        break;
    }
}

static void task_map_chg_seq_0807E20C(u8 taskId)
{
    struct Task * task = &gTasks[taskId];
    s16 *x = &task->data[2];
    s16 *y = &task->data[3];

    switch (task->data[0])
    {
    case 0:
        sub_807DCB0(0);
        FreezeObjectEvents();
        PlayerGetDestCoords(x, y);
        task->data[0] = 1;
        break;
    case 1:
        if (FieldFadeTransitionBackgroundEffectIsFinished())
        {
            sub_807DCB0(TRUE);
            ObjectEventSetHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(0xFF, 0, 0)], GetWalkNormalMovementAction(GetPlayerFacingDirection()));
            task->data[0] = 2;
        }
        break;
    case 2:
        if (walkrun_is_standing_still())
        {
            task->data[0] = 3;
        }
        break;
    case 3:
        UnfreezeObjectEvents();
        ScriptContext2_Disable();
        DestroyTask(taskId);
        break;
    }
}

static void task_map_chg_seq_0807E2CC(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        FreezeObjectEvents();
        ScriptContext2_Enable();
        gTasks[taskId].data[0]++;
        break;
    case 1:
        if (FieldFadeTransitionBackgroundEffectIsFinished())
        {
            UnfreezeObjectEvents();
            ScriptContext2_Disable();
            DestroyTask(taskId);
        }
        break;
    }
}

static void Task_TeleportWarpIn(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        FreezeObjectEvents();
        ScriptContext2_Enable();
        StartTeleportInPlayerAnim();
        gTasks[taskId].data[0]++;
        break;
    case 1:
        if (FieldFadeTransitionBackgroundEffectIsFinished() && WaitTeleportInPlayerAnim() != TRUE)
        {
            UnfreezeObjectEvents();
            ScriptContext2_Disable();
            DestroyTask(taskId);
        }
        break;
    }
}

static void Task_WaitFadeAndCreateStartMenuTask(u8 taskId)
{
    if (FieldFadeTransitionBackgroundEffectIsFinished() == TRUE)
    {
        DestroyTask(taskId);
        CreateTask(Task_StartMenuHandleInput, 80);
    }
}

void FadeTransition_FadeInOnReturnToStartMenu(void)
{
    FadeInFromBlack();
    CreateTask(Task_WaitFadeAndCreateStartMenuTask, 80);
    ScriptContext2_Enable();
}

bool8 FieldCB2_ReturnToStartMenuInit(void)
{
    SetUpReturnToStartMenu();
    return FALSE;
}

static void task_mpl_807E3C8(u8 taskId)
{
    if (FieldFadeTransitionBackgroundEffectIsFinished() == TRUE)
    {
        ScriptContext2_Disable();
        DestroyTask(taskId);
        sub_80696C0();
    }
}

void sub_807E3EC(void)
{
    ScriptContext2_Enable();
    Overworld_PlaySpecialMapMusic();
    FadeInFromBlack();
    CreateTask(task_mpl_807E3C8, 10);
}

static bool32 WaitWarpFadeOutScreen(void)
{
    return gPaletteFade.active;
}

bool32 FieldFadeTransitionBackgroundEffectIsFinished(void)
{
    if (IsWeatherNotFadingIn() == TRUE && ForestMapPreviewScreenIsRunning())
        return TRUE;
    else
        return FALSE;
}

void DoWarp(void)
{
    ScriptContext2_Enable();
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
    PlayRainStoppingSoundEffect();
    PlaySE(SE_KAIDAN);
    gFieldCallback = FieldCB_UnionRoomWarp;
    CreateTask(sub_807E718, 10);
}

void DoDiveWarp(void)
{
    ScriptContext2_Enable();
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
    PlayRainStoppingSoundEffect();
    gFieldCallback = FieldCB_UnionRoomWarp;
    CreateTask(sub_807E718, 10);
}

void sub_807E4A0(u16 metatileBehavior, u16 delay)
{
    u8 taskId = CreateTask(sub_807E980, 10);
    gTasks[taskId].data[1] = metatileBehavior;
    gTasks[taskId].data[15] = delay;
    sub_807E980(taskId);
}

void DoDoorWarp(void)
{
    ScriptContext2_Enable();
    gFieldCallback = FieldCB_UnionRoomWarp;
    CreateTask(sub_807E80C, 10);
}

void sub_807E500(void)
{
    ScriptContext2_Enable();
    CreateTask(sub_807E718, 10);
    gFieldCallback = FieldCB_TeleportWarpIn;
}

void DoUnionRoomWarp(void)
{
    ScriptContext2_Enable();
    gFieldCallback = FieldCB_UnionRoomWarp;
    CreateTask(Task_TeleportWarp, 10);
}

void DoFallWarp(void)
{
    DoDiveWarp();
    gFieldCallback = FieldCB_FallWarpExit;
}

void DoEscalatorWarp(u8 metatileBehavior)
{
    ScriptContext2_Enable();
    StartEscalatorWarp(metatileBehavior, 10);
}

void DoLavaridgeGymB1FWarp(void)
{
    ScriptContext2_Enable();
    StartLavaridgeGymB1FWarp(10);
}

void DoLavaridgeGym1FWarp(void)
{
    ScriptContext2_Enable();
    StartLavaridgeGym1FWarp(10);
}

void DoTeleportWarp(void)
{
    ScriptContext2_Enable();
    TryFadeOutOldMapMusic();
    CreateTask(Task_TeleportWarp, 10);
    gFieldCallback = FieldCB_TeleportWarpIn;
}

void sub_807E5C4(void)
{
    ScriptContext2_Enable();
    WarpFadeOutScreen();
    CreateTask(sub_807E718, 10);
    gFieldCallback = nullsub_60;
}

static void sub_807E5EC(u8 taskId)
{
    struct Task * task = &gTasks[taskId];
    switch (task->data[0])
    {
    case 0:
        ScriptContext2_Enable();
        task->data[0]++;
        break;
    case 1:
        if (!WaitWarpFadeOutScreen() && BGMusicStopped())
            task->data[0]++;
        break;
    case 2:
        WarpIntoMap();
        SetMainCallback2(CB2_ReturnToFieldCableClub);
        DestroyTask(taskId);
        break;
    }
}

void DoCableClubWarp(void)
{
    ScriptContext2_Enable();
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
    PlaySE(SE_KAIDAN);
    CreateTask(sub_807E5EC, 10);
}

static void sub_807E678(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    switch (data[0])
    {
    case 0:
        ClearLinkCallback_2();
        FadeScreen(FADE_TO_BLACK, 0);
        TryFadeOutOldMapMusic();
        PlaySE(SE_KAIDAN);
        data[0]++;
        break;
    case 1:
        if (!WaitWarpFadeOutScreen() && BGMusicStopped())
        {
            Link_TryStartSend5FFF();
            data[0]++;
        }
        break;
    case 2:
        if (gReceivedRemoteLinkPlayers == 0)
        {
            WarpIntoMap();
            SetMainCallback2(CB2_LoadMap);
            DestroyTask(taskId);
        }
        break;
    }
}

void ReturnFromLinkRoom(void)
{
    CreateTask(sub_807E678, 10);
}

static void sub_807E718(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[0])
    {
    case 0:
        FreezeObjectEvents();
        ScriptContext2_Enable();
        task->data[0]++;
        break;
    case 1:
        if (!WaitWarpFadeOutScreen() && BGMusicStopped())
            task->data[0]++;
        break;
    case 2:
        WarpIntoMap();
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(taskId);
        break;
    }
}

static void Task_TeleportWarp(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[0])
    {
    case 0:
        FreezeObjectEvents();
        ScriptContext2_Enable();
        PlaySE(SE_TK_WARPIN);
        StartTeleportWarpOutPlayerAnim();
        task->data[0]++;
        break;
    case 1:
        if (!WaitTeleportWarpOutPlayerAnim())
        {
            WarpFadeOutScreen();
            task->data[0]++;
        }
        break;
    case 2:
        if (!WaitWarpFadeOutScreen() && BGMusicStopped())
            task->data[0]++;
        break;
    case 3:
        WarpIntoMap();
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(taskId);
        break;
    }
}

static void sub_807E80C(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    s16 * xp = &task->data[2];
    s16 * yp = &task->data[3];
    switch (task->data[0])
    {
    case 0:
        FreezeObjectEvents();
        PlayerGetDestCoords(xp, yp);
        PlaySE(GetDoorSoundEffect(*xp, *yp - 1));
        task->data[1] = FieldAnimateDoorOpen(*xp, *yp - 1);
        task->data[0] = 1;
        break;
    case 1:
        if (task->data[1] < 0 || gTasks[task->data[1]].isActive != TRUE)
        {
            ObjectEventClearHeldMovementIfActive(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(0xFF, 0, 0)]);
            ObjectEventSetHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(0xFF, 0, 0)], MOVEMENT_ACTION_WALK_NORMAL_UP);
            task->data[0] = 2;
        }
        break;
    case 2:
        if (walkrun_is_standing_still())
        {
            task->data[1] = FieldAnimateDoorClose(*xp, *yp - 1);
            ObjectEventClearHeldMovementIfFinished(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(0xFF, 0, 0)]);
            sub_807DCB0(FALSE);
            task->data[0] = 3;
        }
        break;
    case 3:
        if (task->data[1] < 0 || gTasks[task->data[1]].isActive != TRUE)
        {
            task->data[0] = 4;
        }
        break;
    case 4:
        TryFadeOutOldMapMusic();
        WarpFadeOutScreen();
        PlayRainStoppingSoundEffect();
        task->data[0] = 0;
        task->func = sub_807E718;
        break;
    case 5:
        TryFadeOutOldMapMusic();
        PlayRainStoppingSoundEffect();
        task->data[0] = 0;
        task->func = sub_807E718;
        break;
    }
}

static void sub_807E980(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    struct ObjectEvent *playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    struct Sprite *playerSpr = &gSprites[gPlayerAvatar.spriteId];
    switch (data[0])
    {
    case 0:
        ScriptContext2_Enable();
        FreezeObjectEvents();
        CameraObjectReset2();
        data[0]++;
        break;
    case 1:
        if (!ObjectEventIsMovementOverridden(playerObj) || ObjectEventClearHeldMovementIfFinished(playerObj))
        {
            if (data[15] != 0)
                data[15]--;
            else
            {
                TryFadeOutOldMapMusic();
                PlayRainStoppingSoundEffect();
                playerSpr->oam.priority = 1;
                sub_807EB64(data[1], &data[2], &data[3]);
                PlaySE(SE_KAIDAN);
                data[0]++;
            }
        }
        break;
    case 2:
        sub_807EAC4(data[2], data[3], &data[4], &data[5], &data[6]);
        data[15]++;
        if (data[15] >= 12)
        {
            WarpFadeOutScreen();
            data[0]++;
        }
        break;
    case 3:
        sub_807EAC4(data[2], data[3], &data[4], &data[5], &data[6]);
        if (!WaitWarpFadeOutScreen() && BGMusicStopped())
            data[0]++;
        break;
    default:
        gFieldCallback = FieldCB_UnionRoomWarp;
        WarpIntoMap();
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(taskId);
        break;
    }
}

static void sub_807EAC4(s16 a0, s16 a1, s16 *a2, s16 *a3, s16 *a4)
{
    struct Sprite *playerSpr = &gSprites[gPlayerAvatar.spriteId];
    struct ObjectEvent *playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (a1 > 0 || *a4 > 6)
        *a3 += a1;
    *a2 += a0;
    (*a4)++;
    playerSpr->pos2.x = *a2 >> 5;
    playerSpr->pos2.y = *a3 >> 5;
    if (playerObj->heldMovementFinished)
    {
        ObjectEventForceSetHeldMovement(playerObj, GetWalkInPlaceNormalMovementAction(GetPlayerFacingDirection()));
    }
}

static void sub_807EB64(u16 a0, s16 *a1, s16 *a2)
{
    ObjectEventForceSetHeldMovement(&gObjectEvents[gPlayerAvatar.objectEventId], GetWalkInPlaceNormalMovementAction(GetPlayerFacingDirection()));
    sub_807EBBC(a0, a1, a2);
}

static void sub_807EBBC(u8 a0, s16 *a1, s16 *a2)
{
    if (MetatileBehavior_IsUnknownWarp6C(a0))
    {
        *a1 = 16;
        *a2 = -10;
    }
    else if (MetatileBehavior_IsUnknownWarp6D(a0))
    {
        *a1 = -17;
        *a2 = -10;
    }
    else if (MetatileBehavior_IsUnknownWarp6E(a0))
    {
        *a1 = 17;
        *a2 = 3;
    }
    else if (MetatileBehavior_IsUnknownWarp6F(a0))
    {
        *a1 = -17;
        *a2 = 3;
    }
    else
    {
        *a1 = 0;
        *a2 = 0;
    }
}

static void sub_807EC34(u8 taskId)
{
    s16 * data = gTasks[taskId].data;
    switch (data[0])
    {
    default:
        if (FieldFadeTransitionBackgroundEffectIsFinished() == TRUE)
        {
            CameraObjectReset1();
            ScriptContext2_Disable();
            DestroyTask(taskId);
        }
        break;
    case 0:
        Overworld_PlaySpecialMapMusic();
        WarpFadeInScreen();
        ScriptContext2_Enable();
        sub_807ECBC(&data[1], &data[2], &data[3], &data[4], &data[5]);
        data[0]++;
        break;
    case 1:
        if (!sub_807EDA0(&data[1], &data[2], &data[3], &data[4], &data[5]))
            data[0]++;
        break;
    }
}

static void sub_807ECBC(s16 *a0, s16 *a1, s16 *a2, s16 *a3, s16 *a4)
{
    s16 x, y;
    u8 behavior;
    s32 r1;
    struct Sprite *sprite;
    PlayerGetDestCoords(&x, &y);
    behavior = MapGridGetMetatileBehaviorAt(x, y);
    if (MetatileBehavior_IsUnknownWarp6E(behavior) || MetatileBehavior_IsUnknownWarp6C(behavior))
        r1 = 3;
    else
        r1 = 4;
    ObjectEventForceSetHeldMovement(&gObjectEvents[gPlayerAvatar.objectEventId], GetWalkInPlaceSlowMovementAction(r1));
    sub_807EBBC(behavior, a0, a1);
    *a2 = *a0 * 16;
    *a3 = *a1 * 16;
    *a4 = 16;
    sprite = &gSprites[gPlayerAvatar.spriteId];
    sprite->pos2.x = *a2 >> 5;
    sprite->pos2.y = *a3 >> 5;
    *a0 *= -1;
    *a1 *= -1;
}

static bool8 sub_807EDA0(s16 *a0, s16 *a1, s16 *a2, s16 *a3, s16 *a4)
{
    struct Sprite *sprite;
    sprite = &gSprites[gPlayerAvatar.spriteId];
    if (*a4 != 0)
    {
        *a2 += *a0;
        *a3 += *a1;
        sprite->pos2.x = *a2 >> 5;
        sprite->pos2.y = *a3 >> 5;
        (*a4)--;
        return TRUE;
    }
    else
    {
        sprite->pos2.x = 0;
        sprite->pos2.y = 0;
        return FALSE;
    }
}
