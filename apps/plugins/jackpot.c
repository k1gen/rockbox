/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2007 Copyright Kévin Ferrare based on work by Pierre Delore
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "plugin.h"
#include "lib/pluginlib_actions.h"
#include "lib/picture.h"
#include "lib/pluginlib_exit.h"


#if (CONFIG_KEYPAD == IPOD_1G2G_PAD) \
    || (CONFIG_KEYPAD == IPOD_3G_PAD) \
    || (CONFIG_KEYPAD == IPOD_4G_PAD)
#define JACKPOT_QUIT PLA_UP
#else
#define JACKPOT_QUIT PLA_CANCEL
#endif

const struct button_mapping* plugin_contexts[]={pla_main_ctx};
#define NB_PICTURES 9
#define NB_SLOTS 3

#define PICTURE_HEIGHT (BMPHEIGHT_jackpot_slots/(NB_PICTURES+1))
#if NB_SCREENS==1
#define PICTURE_ROTATION_STEPS PICTURE_HEIGHT
#else
#define REMOTE_PICTURE_HEIGHT (BMPHEIGHT_jackpot_slots_remote/(NB_PICTURES+1))
#define PICTURE_ROTATION_STEPS REMOTE_PICTURE_HEIGHT
#endif

/* FIXME: would be nice to have better graphics ... */
#include "pluginbitmaps/jackpot_slots.h"
#if NB_SCREENS==2
#include "pluginbitmaps/jackpot_slots_remote.h"
#endif

const struct picture jackpot_pictures[]={
    {jackpot_slots, BMPWIDTH_jackpot_slots, BMPHEIGHT_jackpot_slots, 
            PICTURE_HEIGHT},
#if NB_SCREENS==2
    {jackpot_slots_remote,BMPWIDTH_jackpot_slots_remote, 
            BMPHEIGHT_jackpot_slots_remote, REMOTE_PICTURE_HEIGHT}
#endif
};

#define SLEEP_TIME (HZ/200)

struct jackpot
{
    /* A slot can display "NB_PICTURES" pictures
       A picture is moving up, it can take PICTURE_ROTATION_STEPS
       to move a single picture completely.
       So values in slot_state are comprised between
       0 and NB_PICTURES*PICTURE_ROTATION_STEPS
     */
    int slot_state[NB_SLOTS];
    /*
       The real state of the picture in pixels on each screen
       Different from slot_state because of the synchronised
       rotation between different sized bitmaps on remote and main screen
     */
    int state_y[NB_SCREENS][NB_SLOTS];
    int money;
};

/*Call when the program exit*/
static void jackpot_exit(void)
{
}

static void jackpot_init(struct jackpot* game)
{
    game->money=20;
    for(int i=0;i<NB_SLOTS;i++){
        game->slot_state[i]=(rb->rand()%NB_PICTURES)*PICTURE_ROTATION_STEPS;
        FOR_NB_SCREENS(j)
            game->state_y[j][i]=-1;
    }
}

static int jackpot_get_result(struct jackpot* game)
{
    int i=NB_SLOTS-1;
    int multiple=1;
    int result=0;
    for(;i>=0;i--)
    {
        result+=game->slot_state[i]*multiple/PICTURE_ROTATION_STEPS;
        multiple*=10;
    }
    return(result);
}

static int jackpot_get_gain(struct jackpot* game)
{
    switch (jackpot_get_result(game))
    {
        case 111 : return(20);
        case 000 : return(15);
        case 333 : return(10);
        case 222 : return(8);
        case 555 : return(5);
        case 777 : return(4);
        case 251 : return(4);
        case 510 : return(4);
        case 686 : return(3);
        case 585 : return(3);
        case 282 : return(3);
        case 484 : return(3);
        case 787 : return(2);
        case 383 : return(2);
        case 80  : return(2);
    }
    return(0);
}

static void jackpot_display_slot_machine(struct jackpot* game, struct screen* display)
{
    char str[20];
    int i;
    bool changes=false;
    const struct picture* picture= &(jackpot_pictures[display->screen_type]);
    int pos_x=(display->getwidth()-NB_SLOTS*(picture->width+1))/2;
    int pos_y=(display->getheight()-(picture->slide_height))/2;
    for(i=0;i<NB_SLOTS;i++)
    {
        int state_y=
                (picture->slide_height*game->slot_state[i])/PICTURE_ROTATION_STEPS;
        int previous_state_y=game->state_y[display->screen_type][i];
        if(state_y==previous_state_y)
            continue;/*no need to update the picture
                       as it's the same as previous displayed one*/
        changes=true;
        game->state_y[display->screen_type][i]=state_y;
        vertical_picture_draw_part(display, picture, state_y, pos_x, pos_y);
        pos_x+=(picture->width+1);
    }
    if(changes){
        rb->snprintf(str,sizeof(str),"money : $%d", game->money);
        display->puts(0, 0, str);
        display->update();
    }
}


static void jackpot_info_message(struct screen* display, char* message)
{
    int xpos, ypos;
    int message_height, message_width;
    display->getstringsize(message, &message_width, &message_height);
    xpos=(display->getwidth()-message_width)/2;
    ypos=display->getheight()-message_height;
    rb->screen_clear_area(display, 0, ypos, display->getwidth(),
                          message_height);
    display->putsxy(xpos,ypos,message);
    display->update();
}

static void jackpot_print_turn_result(struct jackpot* game,
                               int gain, struct screen* display)
{
    char str[20];
    if (gain==0)
    {
        jackpot_info_message(display, "None ...");
        if (game->money<=0)
            jackpot_info_message(display, "You lose...STOP to quit");
    }
    else
    {
        rb->snprintf(str,sizeof(str),"You win %d$",gain);
        jackpot_info_message(display, str);
    }
    display->update();
}

static void jackpot_play_turn(struct jackpot* game)
{
    /* How many pattern? */
    int nb_turns[NB_SLOTS];
    int gain,turns_remaining=0;
    if(game->money<=0)
        return;
    game->money--;
    for(int i=0;i<NB_SLOTS;i++)
    {
        nb_turns[i]=(rb->rand()%15+5)*PICTURE_ROTATION_STEPS;
        turns_remaining+=nb_turns[i];
    }
    FOR_NB_SCREENS(d)
    {
        rb->screens[d]->clear_display();
        jackpot_info_message(rb->screens[d],"Good luck");
    }
    /* Jackpot Animation */
    while(turns_remaining>0)
    {
        for(int i=0;i<NB_SLOTS;i++)
        {
            if(nb_turns[i]>0)
            {
                nb_turns[i]--;
                game->slot_state[i]++;
                if(game->slot_state[i]>=PICTURE_ROTATION_STEPS*NB_PICTURES)
                    game->slot_state[i]=0;
                turns_remaining--;
            }
        }
        FOR_NB_SCREENS(d)
            jackpot_display_slot_machine(game, rb->screens[d]);
        rb->sleep(SLEEP_TIME);
    }
    gain=jackpot_get_gain(game);
    if(gain!=0)
        game->money+=gain;
    FOR_NB_SCREENS(d)
        jackpot_print_turn_result(game, gain, rb->screens[d]);
}

enum plugin_status plugin_start(const void* parameter)
{
    int action;
    struct jackpot game;
    (void)parameter;
    atexit(jackpot_exit);
    rb->srand(*rb->current_tick);
    jackpot_init(&game);

    FOR_NB_SCREENS(i){
        rb->screens[i]->clear_display();
        jackpot_display_slot_machine(&game, rb->screens[i]);
    }
    /*Empty the event queue*/
    rb->button_clear_queue();
    while (true)
    {
        action = pluginlib_getaction(TIMEOUT_BLOCK,
                                plugin_contexts, ARRAYLEN(plugin_contexts));
        switch ( action )
        {
            case JACKPOT_QUIT:
                return PLUGIN_OK;
            case PLA_SELECT:
                jackpot_play_turn(&game);
                break;

            default:
                exit_on_usb(action);
                break;
        }
    }
    return PLUGIN_OK;
}
