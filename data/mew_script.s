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

@ You can call this function (via ACE, etc)
@ to set up the Mew event in the save's ramScript
	.align	2, 0
	.globl	SetMewEventToRamScript
	.type	 SetMewEventToRamScript,function
	.thumb_func
SetMewEventToRamScript: @ 09000001
	@ preserve registers except r0,
	@ which we zero out (for easy ACE exits)
	push	{r1, r2, r3, r4, lr}
	add	sp, sp, #-0x4
	ldr	r0, .L641
	ldr	r1, .L641+0x4
	sub	r1, r1, r0
	lsl	r1, r1, #0x10
	lsr	r1, r1, #0x10
	mov	r2, #0x6
	str	r2, [sp]
	mov	r2, #0x3
	mov	r3, #0x5
	ldr	r4, pool_InitRamScript
	bl	_custom_call_via_r4
	lsl	r0, r0, #0x18
	cmp	r0, #0
	bne	.L640	@cond_branch
	@ halt if there isn't enough space
	.hword 0xEFFF
	.code	16
.L640:
	add	sp, sp, #0x4
	mov	r0, #0
	pop	{r1, r2, r3, r4, pc}
_custom_call_via_r4:
	bx	r4
.L642:
	.align	2, 0
.L641:
	.word	Mew_EventScript_Sailor
	.word	Mew_Script_End
pool_InitRamScript: .word InitRamScript
	.word	0x15151515 @ marker (151 for Mew ;)
.Lfe128:
	.size	 SetMewEventToRamScript,.Lfe128-SetMewEventToRamScript

@ Event is triggered by talking to the sailor in Vermilion
@ after retrieving HM01 from the captain
	.align	2, 0
Mew_EventScript_Sailor::
	setvaddress Mew_EventScript_Sailor
	goto_if_set FLAG_CAUGHT_MEW, VermilionCity_EventScript_FerrySailor
	goto_if_unset FLAG_GOT_HM01, VermilionCity_EventScript_FerrySailor
	lock
	faceplayer
	@ remove vermilion triggers
	setvar VAR_TEMP_1, 1
	setvar VAR_VERMILION_CITY_TICKET_CHECK_TRIGGER, 1
	vmessage Mew_Text_TruckSomething
	waitmessage
	waitbuttonpress
	vcall Mew_EventScript_Setup
	callnative SetVBlankIntr
	release
	end

Mew_Text_TruckSomething:
	.string "Something's been disturbing all of\n"
	.string "our shipments lately…\l"
	.string "Could it be a rare POKéMON?$"

@ Code

@	.align	2, 0
@	.globl	CopyScriptPtr
@	.type	 CopyScriptPtr,function
@	.thumb_func
@CopyScriptPtr:
@	ldr	r0, [r4, #0x08] @ r0 = scriptPtr
@	ldr	r1, [r4, #0x64] @ r1 = ctx->data[0]
@	mov	r2, #255 @ wordcount
@	swi	12 @ CpuFastSet
@	bx	lr

	.align 2, 0
Mew_EventScript_Setup::
	@ Write CopyScriptPtr byte-by-byte
	setptr 0xA0, 0x0203FBB0
	setptr 0x68, 0x0203FBB1
	setptr 0x61, 0x0203FBB2
	setptr 0x6E, 0x0203FBB3
	setptr 0xFF, 0x0203FBB4
	setptr 0x22, 0x0203FBB5
	setptr 0x0C, 0x0203FBB6
	setptr 0xDF, 0x0203FBB7
	setptr 0x70, 0x0203FBB8
	setptr 0x47, 0x0203FBB9 @ +60
	@ copy code here
	loadword 0, 0x0203FBC0-4 @ +66
	nop @ +67
	callnative 0x0203FBB1 @ CopyScriptPtr @ +72, aligned!
	return @ +73
	return
	return
	return

@ data/ram_script.s immediately follows
