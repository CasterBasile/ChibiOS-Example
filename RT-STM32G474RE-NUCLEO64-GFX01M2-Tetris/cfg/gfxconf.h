

#ifndef _GFXCONF_H
#define _GFXCONF_H

#define GFX_USE_OS_CHIBIOS                           GFXON
#define GFX_MEM_LT64K                                GFXOFF
/* Features for the GDISP sub-system. */
#define GDISP_USE_DMA   GFXON
#define GDISP_NEED_CONTROL  GFXON
#define GDISP_NEED_VALIDATION GFXON
#define GDISP_NEED_CLIP   GFXON
#define GDISP_NEED_TEXT   GFXON
#define GDISP_NEED_CIRCLE  GFXON
#define GDISP_NEED_MULTITHREAD GFXON

/* Comment these out if it conflicts with your hardware */
#define GDISP_NEED_CONTROL  GFXON
#define GDISP_DEFAULT_ORIENTATION    gOrientationPortrait
#define GDISP_SCREEN_WIDTH      240
#define GDISP_SCREEN_HEIGHT     320

/* Builtin Fonts */
#define GDISP_INCLUDE_FONT_DEJAVUSANS12  GFXON
#define GDISP_INCLUDE_FONT_DEJAVUSANS16  GFXON
#define GDISP_NEED_ANTIALIAS    GFXOFF

/* GFX sub-systems to turn on */
#define GFX_USE_GDISP                                GFXON
#define GFX_USE_GEVENT                               GFXON
#define GFX_USE_GTIMER                               GFXON
#define GFX_USE_GINPUT                               GFXON

/* Builtin Fonts */
#define GDISP_INCLUDE_FONT_DEJAVUSANS12              GFXON
#define GDISP_INCLUDE_FONT_DEJAVUSANS16              GFXON

#define GWIN_NEED_WIDGET                             GFXON
#define GWIN_NEED_BUTTON                             GFXON
#define GWIN_BUTTON_LAZY_RELEASE                     GFXON

/* Comment these out if it conflicts with your hardware */

#define GDISP_DEFAULT_ORIENTATION                   gOrientationPortrait
#define GDISP_SCREEN_WIDTH                          240
#define GDISP_SCREEN_HEIGHT                         320




///////////////////////////////////////////////////////////////////////////
// GWIN                                                                  //
///////////////////////////////////////////////////////////////////////////
//#define GFX_USE_GWIN                                 GFXOFF

#define GWIN_NEED_WIDGET                             GFXON
#define GWIN_NEED_BUTTON                             GFXON
#define GWIN_BUTTON_LAZY_RELEASE                     GFXON
#define GWIN_NEED_SLIDER                             GFXON
//        #define GWIN_SLIDER_NOSNAP                   GFXOFF
//        #define GWIN_SLIDER_DEAD_BAND                5
//        #define GWIN_SLIDER_TOGGLE_INC               20
//    #define GWIN_NEED_CHECKBOX                       GFXOFF
//    #define GWIN_NEED_IMAGE                          GFXOFF
//        #define GWIN_NEED_IMAGE_ANIMATION            GFXOFF
//    #define GWIN_NEED_RADIO                          GFXOFF
//    #define GWIN_NEED_LIST                           GFXOFF
//        #define GWIN_NEED_LIST_IMAGES                GFXOFF
//    #define GWIN_NEED_PROGRESSBAR                    GFXOFF
//        #define GWIN_PROGRESSBAR_AUTO                GFXOFF
//    #define GWIN_NEED_KEYBOARD                       GFXOFF
//        #define GWIN_KEYBOARD_DEFAULT_LAYOUT         VirtualKeyboard_English1
//        #define GWIN_NEED_KEYBOARD_ENGLISH1          GFXON
//    #define GWIN_NEED_TEXTEDIT                       GFXOFF
//    #define GWIN_FLAT_STYLING                        GFXOFF
//    #define GWIN_WIDGET_TAGS                         GFXOFF

//#define GWIN_NEED_CONTAINERS                         GFXOFF
//    #define GWIN_NEED_CONTAINER                      GFXOFF
//    #define GWIN_NEED_FRAME                          GFXOFF
//    #define GWIN_NEED_TABSET                         GFXOFF
//        #define GWIN_TABSET_TABHEIGHT                18


///////////////////////////////////////////////////////////////////////////
// GTRANS                                                                //
///////////////////////////////////////////////////////////////////////////
//#define GFX_USE_GTRANS                               GFXOFF

///////////////////////////////////////////////////////////////////////////
// GEVENT                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GEVENT                                 GFXON
#define GEVENT_ASSERT_NO_RESOURCE                      GFXON
#define GEVENT_MAXIMUM_SIZE                             32
#define GEVENT_MAX_SOURCE_LISTENERS                     32
///////////////////////////////////////////////////////////////////////////
// GINPUT                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GINPUT                                 GFXON
#define GINPUT_NEED_TOGGLE                             GFXON

#define GINPUT_TOGGLE_CONFIG_ENTRIES 5  // Quanti pulsanti stai usando
#define GINPUT_TOGGLE_NUM_PORTS      1  // Di solito 1 se usi solo GPIO
//#define GINPUT_NEED_DIAL                             GFXOFF



#endif /* _GFXCONF_H */
