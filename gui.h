/*
 *  gui.h: An ncurses GUI for dwipe.
 *
 *  Copyright Darik Horn <dajhorn-dban@vanadac.com>.
 *
 *  This program is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation, version 2.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 *  Ave, Cambridge, MA 02139, USA.
 *
 */


#ifndef GUI_H_
#define GUI_H_

void dwipe_gui_free( void );                             /* Stop the GUI.              */
void dwipe_gui_init( void );                             /* Start the GUI.             */
void dwipe_gui_select( int count, dwipe_context_t* c );  /* Select devices to wipe.    */
void dwipe_gui_status( int count, dwipe_context_t* c );  /* Update operation progress. */
void dwipe_gui_method( void );                           /* Change the method option.  */
void dwipe_gui_options( void );                          /* Update the options window. */
void dwipe_gui_prng( void );                             /* Change the prng option.    */
void dwipe_gui_rounds( void );                           /* Change the rounds option.  */
void dwipe_gui_verify( void );                           /* Change the verify option.  */

void dwipe_update_speedring( dwipe_speedring_t* speedring, u64 speedring_done, time_t speedring_now );

#endif /* GUI_H_ */

/* eof */
