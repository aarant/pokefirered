#include "constants/global.h"
#include "constants/flags.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/decorations.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/maps.h"
#include "constants/metatile_labels.h"
#include "constants/pokemon.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/sound.h"
#include "constants/species.h"
#include "constants/vars.h"
#include "constants/battle.h"
#include "constants/heal_locations.h"
#include "constants/field_effects.h"
#include "constants/trainers.h"
#include "constants/trainer_tower.h"
#include "constants/fame_checker.h"
#include "constants/seagallop.h"
#include "constants/game_stat.h"
#include "constants/coins.h"
#include "constants/menu.h"
#include "constants/battle_setup.h"
#include "constants/map_scripts.h"
#include "constants/cable_club.h"
#include "constants/field_tasks.h"
#include "constants/field_weather.h"
#include "constants/weather.h"
#include "constants/union_room.h"
#include "constants/trade.h"
#include "constants/quest_log.h"
#include "constants/daycare.h"
#include "constants/easy_chat.h"
#include "constants/trainer_card.h"
#include "constants/help_system.h"
#include "constants/trainer_fan_club.h"
#include "constants/mystery_gift.h"
	.include "asm/macros.inc"
	.include "asm/macros/event.inc"
	.set FALSE, 0
	.set TRUE,  1

	.code	16
	.text

	.align	2, 0
	.globl	SetVBlankIntr
	.type	 SetVBlankIntr,function
	.thumb_func
SetVBlankIntr:
	ldr	r1, .L88
	adr	r0, VBlankIntr_CustomMew
	add	r0, #0x1
	str	r0, [r1, #0x10]
	bx	lr
.L89:
	.align	2, 0
.L88:
	.word	gIntrTable
.Lfe16:
	.size	 SetVBlankIntr,.Lfe16-SetVBlankIntr

	.align	2, 0
	.globl	VBlankIntr_CustomMew
	.type	 VBlankIntr_CustomMew,function
	.thumb_func
VBlankIntr_CustomMew:
	push	{lr}
	ldr	r0, .L95
	ldr	r0, [r0]
	ldrh	r1, [r0, #0x4]
	ldr	r0, .L95+0x4
	cmp	r1, r0
	bne	.L91	@cond_branch
	ldr	r2, .L95+0x8
	ldr	r0, [r2, #0x4]
	adr	r1, Mew_MapEvents
	cmp	r0, r1
	beq	.L92	@cond_branch
	ldr	r0, [r0, #0x8]
	str	r1, [r2, #0x4]
	str	r0, [r1, #0x8]
	ldr	r0, pool_LoadObjEventTemplatesFromHeader
	bl _custom_call_via_r0
	b	.L91
.L96:
	.align	2, 0
.L95:
	.word	gSaveBlock1Ptr
	.word	0x401
	.word	gMapHeader
pool_LoadObjEventTemplatesFromHeader: .word LoadObjEventTemplatesFromHeader
.L92:
	ldr	r2, .L97
	ldr	r1, [r2, #0x4]
	ldr	r0, .L97+0x4
	cmp	r1, r0
	bne	.L91	@cond_branch
	mov	r1, #0x87
	lsl	r1, r1, #0x3
	add	r0, r2, r1
	ldrb	r0, [r0]
	cmp	r0, #0x1
	bne	.L91	@cond_branch
	ldr	r0, .L97+0x8
	ldr	r1, [r0]
	ldr	r3, .L97+0xc
	add	r2, r1, r3
	adr	r0, sJapaneseNamingScreenTemplate
	str	r0, [r2]
	sub	r3, #0x12
	add	r1, r1, r3
	mov	r0, #0x6a
	strh	r0, [r1]
.L91:
	@ can't `pop {pc}`, because rtn addr is ARM!
	pop	{r0}
	mov	lr, r0
	@ tailcall
	ldr	r0, pool_VBlankIntr
_custom_call_via_r0:
	bx	r0
.L98:
	.align	2, 0
.L97:
	.word	gMain
	.word	CB2_LoadNamingScreen
	.word	sNamingScreen
	.word	0x1e28
pool_VBlankIntr: .word VBlankIntr
sJapaneseNamingScreenTemplate:
	.byte	0 @ copyExistingString
	.byte	5 @ maxChars
	.byte	3 @ iconFunction
	.byte	1 @ addGenderIcon
	.byte	1 @ initialPage
	.space 3
	.word	gText_PkmnsNickname @ title
.Lfe17:
	.size	 VBlankIntr_CustomMew,.Lfe17-VBlankIntr_CustomMew

@.set LOCALID_SS_ANNE, 1
.set LOCALID_TRUCK, 1
.set LOCALID_MEW, 2

.align 2
Mew_MapEvents::
    .byte 3 @ OW count
    .byte 2 @ warp count
    .byte 0 @ coordEventCount
    .byte 0 @ bgEventCount
    .4byte Mew_EventObjects
    .4byte 0 @ warps (copied from SSAnne map)
    @.4byte 0 @ coordEvents
    @.4byte 0 @ bgEvents
.align 2
Mew_EventObjects::
    @object_event LOCALID_SS_ANN	E, OBJ_EVENT_GFX_SS_ANNE, 30, 16, 1, MOVEMENT_TYPE_FACE_DOWN, 0, 0, 0, 0, 0, FLAG_HIDE_SS_ANNE
    object_event LOCALID_TRUCK, OBJ_EVENT_GFX_SEAGALLOP, 56, 3, 3, 0, 0, 0, 0, 0, 0, FLAG_HIDE_POKEDEX
    object_event LOCALID_MEW, OBJ_EVENT_GFX_MEW, 56, 3, 3, MOVEMENT_TYPE_INVISIBLE, 0, 0, 0, 0, Mew_EventScript_Truck, FLAG_CAUGHT_MEW

Mew_EventScript_Truck:
	lockall
	checkpartymove MOVE_STRENGTH
	goto_if_eq VAR_RESULT, PARTY_SIZE, EventScript_CantMoveBoulder
	setfieldeffectargument 0, VAR_RESULT
	msgbox Text_UseStrength, MSGBOX_YESNO @ TODO: Truck, not boulder?
	goto_if_eq VAR_RESULT, NO, EventScript_DontUseStrength
	closemessage
	dofieldeffect FLDEFF_USE_STRENGTH
	waitstate
	delay 30
	addobject LOCALID_TRUCK
	callnative LoadTruckGfx
Mew_EventScript_HideOldTruck:
	setmetatile 55, 1, 0x12B, TRUE
	setmetatile 56, 1, 0x12B, TRUE

	setmetatile 55, 2, 0x31B, FALSE
	setmetatile 56, 2, 0x31B, FALSE
	setmetatile 57, 2, 0x31B, FALSE

	setmetatile 55, 3, 0x1C1, FALSE
	setmetatile 56, 3, 0x1C1, FALSE
	setmetatile 57, 3, 0x1C1, FALSE
	special DrawWholeMapView

	playse SE_M_STRENGTH
	applymovement LOCALID_TRUCK, Mew_Movement_Truck
	waitmovement 0
Mew_EventScript_ShowNewTruck:
	setmetatile 53, 1, 0x313, TRUE
	setmetatile 54, 1, 0x314, TRUE

	setmetatile 53, 2, 0x315, TRUE
	setmetatile 54, 2, 0x316, TRUE
	setmetatile 55, 2, 0x317, TRUE

	setmetatile 53, 3, 0x318, TRUE
	setmetatile 54, 3, 0x319, TRUE
	setmetatile 55, 3, 0x31A, TRUE
	special DrawWholeMapView

	removeobject LOCALID_TRUCK
	delay 60
	applymovement LOCALID_MEW, Mew_Movement_Mew
	waitmovement 0
	playmoncry SPECIES_MEW, CRY_MODE_NORMAL
	delay 60
	seteventmon SPECIES_MEW, 30
	callnative StartLegendaryBattleMew
	delay 64
	setobjectxy LOCALID_MEW, 0, 0 @ hide mew after battle
	waitstate
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Mew_EventScript_MewFled
	setflag FLAG_CAUGHT_MEW
	releaseall
	end
Mew_EventScript_MewFled:
	bufferspeciesname STR_VAR_1, SPECIES_MEW
	msgbox Text_MonFlewAway
	releaseall
	end

Mew_Movement_Truck:
	walk_left
	walk_left
	step_end

Mew_Movement_Mew:
	walk_up
	face_down
	set_visible
	delay_4
	set_invisible
	delay_4
	set_visible
	delay_4
	set_invisible
	delay_4
	set_visible
	delay_4
	set_invisible
	delay_4
	set_visible
	delay_4
	step_end

@ Code

	.align	2, 0
	.globl	LoadTruckGfx
	.type	 LoadTruckGfx,function
	.thumb_func
LoadTruckGfx:
	push	{r6, r7, lr}
	mov	r0, #0x1 @ LOCALID_TRUCK
	ldr	r7, pool_GetObjectEventIdByLocalId
	bl _custom_call_via_r7
	mov	r1, #0x24
	mul	r1, r0
	ldr	r0, .L2810
	add	r1, r1, r0
	ldrb	r2, [r1, #0x4]
	mov	r0, #0x44
	mul	r0, r2
	ldr	r2, .L2810+0x4
	add	r3, r0, r2
	mov	r0, #0x10 @ inanimate
	strb	r0, [r1, #0x1]
	add	r2, r3, #0
	add	r2, r2, #0x3f
	ldrb	r0, [r2]
	mov	r1, #0x40
	orr	r0, r0, r1
	strb	r0, [r2]
	add	r0, r3, #0
	add	r0, r0, #0x40
	mov	r2, #0xf0
	lsl	r2, r2, #0x2
	strh	r2, [r0]
	ldrh	r1, [r3, #0x4]
	ldr	r0, .L2810+0x8
	and	r0, r0, r1
	orr	r0, r0, r2
	strh	r0, [r3, #0x4]
	ldr	r0, .L2810+0xc
	ldr	r1, .L2810+0x10
	mov	r2, #0x30
	mov	r6, #0xA0
	lsl	r6, r6, #0x1 @ r6 = 0x140
	swi	12 @ CpuFastSet
	add	r0, #0x40 @ldr	r0, .L2810+0x14
	add	r1, #0x40
	swi	12 @ CpuFastSet
	add	r0, r0, r6 @ldr	r0, .L2810+0x18
	add	r1, #0x40
	swi	12 @ CpuFastSet
	sub	r0, r0, r6 @ldr	r0, .L2810+0x1C
	sub	r0, #0xC0
	add	r1, #0x40
	swi	12 @ CpuFastSet
	add	r0, r0, r6 @ldr	r0, .L2810+0x20
	add	r1, #0x40
	swi	12 @ CpuFastSet
	@ Load palette
	ldr	r0, .L2810+0x14
	mov	r2, #0xc0
	lsl	r2, r2, #0x2
	add	r1, r0, r2
	mov	r2, #0x8
	swi	12 @ CpuFastSet
	pop	{r6, r7, pc}
.L2811:
	.align	2, 0
.L2810:
	.word	gObjectEvents
	.word	gSprites
	.word	-0x400
	.word	0x6005d00
	.word	0x6017b20
	.word	gPlttBufferFaded+0x40
pool_GetObjectEventIdByLocalId: .word GetObjectEventIdByLocalId
pool_LoadPalette: .word LoadPalette
.Lfe892:
	.size	 LoadTruckGfx,.Lfe892-LoadTruckGfx


	.align	2, 0
	.globl	StartLegendaryBattleMew
	.type	 StartLegendaryBattleMew,function
	.thumb_func
StartLegendaryBattleMew:
	push	{r4, r7, lr}
	ldr	r7, pool_StartLegendaryBattle
	bl _custom_call_via_r7
	mov	r0, #0xaa
	lsl	r0, r0, #0x1
	ldr	r7, pool_PlayMapChosenOrBattleBGM
	bl	_custom_call_via_r7
	ldr	r4, .L81
	add	r0, r4, #0
	mov	r1, #0x25
	adr	r2, pool_met_game
	ldr	r7, pool_SetMonData
	bl	_custom_call_via_r7
	add	r0, r4, #0
	mov	r1, #0x23
	adr	r2, pool_met_loc
	bl	_custom_call_via_r7
	mov	r0, #0x1 @ LANGUAGE_JAPANESE
	strb	r0, [r4, #0x12]
	mov	r0, #0xff @ EOS
	strb	r0, [r4, #0x19] @ clip otName
	pop	{r4, r7, pc}
.L82:
	.align	2, 0
.L81:
	.word	gEnemyParty
pool_met_game: .word 3 @ VERSION_EMERALD
pool_met_loc: .word 0xC9 @ MAPSEC_FARAWAY_ISLAND
pool_StartLegendaryBattle: .word StartLegendaryBattle
pool_PlayMapChosenOrBattleBGM: .word PlayMapChosenOrBattleBGM
pool_SetMonData: .word SetMonData
.Lfe15:
	.size	 StartLegendaryBattleMew,.Lfe15-StartLegendaryBattleMew

_custom_call_via_r7:
	bx r7
