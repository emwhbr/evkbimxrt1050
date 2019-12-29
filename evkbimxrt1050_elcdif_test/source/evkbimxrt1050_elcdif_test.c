/*
 * evkbimxrt1050_elcdif_test.c
 *
 *  Created on: Dec 29, 2019
 *      Author: emwhbr
 */

#include <stdio.h>

#include "fsl_common.h"
#include "fsl_elcdif.h"
#include "fsl_debug_console.h"

#include "pin_mux.h"
#include "board.h"
#include "fsl_gpio.h"
#include "clock_config.h"

#include "board_extra.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

// Undef this for another demo
#define DRAW_GROWING_BOX
//#undef DRAW_GROWING_BOX

// Define this for using a syntetic load
#undef USE_TICK_LOAD
//#define USE_TICK_LOAD

#define APP_ELCDIF LCDIF

#define APP_IMG_HEIGHT 272
#define APP_IMG_WIDTH  480

#define APP_HSW 1
#define APP_HFP 10
#define APP_HBP 43

#define APP_VSW 13
#define APP_VFP 4
#define APP_VBP 12

#define APP_POL_FLAGS \
    (kELCDIF_DataEnableActiveHigh | \
     kELCDIF_VsyncActiveLow       | \
     kELCDIF_HsyncActiveLow       | \
     kELCDIF_DriveDataOnRisingClkEdge)

// Display
#define LCD_DISP_GPIO GPIO1
#define LCD_DISP_GPIO_PIN 2

// Back light
#define LCD_BL_GPIO GPIO2
#define LCD_BL_GPIO_PIN 31

// Frame buffer data alignment, for better performance,
// the LCDIF frame buffer should be 64B align.
#define FRAME_BUFFER_ALIGN 64

// Size in bytes of one frame buffer
#define FB_PIXEL_SIZE (sizeof(uint32_t))
#define FB_SIZE       (APP_IMG_HEIGHT * APP_IMG_WIDTH * FB_PIXEL_SIZE)

/*******************************************************************************
 * Global variables
 *******************************************************************************/

#if defined USE_TICK_LOAD
static volatile uint32_t g_systickCounter;
#endif

static volatile uint32_t g_frameCounter = 0;

AT_NONCACHEABLE_SECTION_ALIGN(
    static uint8_t g_frameBuffer2[2 * FB_SIZE],
    FRAME_BUFFER_ALIGN);

static volatile bool g_frameDone = false;

/* -------------------------------------------------------------- */

#define APP_LCDIF_IRQHandler LCDIF_IRQHandler

void APP_LCDIF_IRQHandler(void)
{
    uint32_t intStatus = ELCDIF_GetInterruptStatus(APP_ELCDIF);

    ELCDIF_ClearInterruptStatus(APP_ELCDIF, intStatus);

    if (intStatus & kELCDIF_CurFrameDone)
    {
        GPIO_PinWrite(BOARD_J22P7_GPIO, BOARD_J22P7_PIN, 1U);
        g_frameDone = true;
        g_frameCounter++;
    }

    __DSB();
}

#if defined USE_TICK_LOAD

/* -------------------------------------------------------------- */

void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}

/* -------------------------------------------------------------- */

static void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U) {}
}

#endif // USE_TICK_LOAD

/* -------------------------------------------------------------- */

static void APP_InitLcdifPixelClock(void)
{
    /*
     * The desired output frame rate is 40Hz. So the pixel clock frequency is:
     * (480 + 1 + 10 + 43) * (272 + 13 + 4 + 12) * 40 =
     *    534 * 301 * 40 =
     * 160734 * 40 = 6429360Hz = 6.4294MHz
     * Here set the LCDIF pixel clock to 90MHz / 14 = 6.4286MHz
     */

    /*
     * Initialize the Video PLL.
     * Video PLL output clock is OSC24M * (loopDivider + (denominator / numerator)) / postDivider = 90MHz.
      */
    clock_video_pll_config_t config = {
        .loopDivider = 30,
        .postDivider = 8,
        .numerator   = 0,
        .denominator = 0,
    };

    CLOCK_InitVideoPll(&config);

    /*
     * 000 derive clock from PLL2
     * 001 derive clock from PLL3 PFD3
     * 010 derive clock from PLL5
     * 011 derive clock from PLL2 PFD0
     * 100 derive clock from PLL2 PFD1
     * 101 derive clock from PLL3 PFD1
     */
    CLOCK_SetMux(kCLOCK_LcdifPreMux, 2); // Derive clock from PLL5

    // LCDIF_CLOCK = 90MHz / (7 * 2) = 6.4286MHz
    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 6); // Set LCDIF_PRED  /7
    CLOCK_SetDiv(kCLOCK_LcdifDiv, 1);    // Set LCDIF_PODF  /2
}

/* -------------------------------------------------------------- */

static void APP_InitLcd(void)
{
    gpio_pin_config_t config = {
        kGPIO_DigitalOutput,
        0,
    };

    // Backlight
    config.outputLogic = 1;
    GPIO_PinInit(LCD_BL_GPIO, LCD_BL_GPIO_PIN, &config);
}

/* -------------------------------------------------------------- */

static void APP_ELCDIF_Init(void)
{
    const elcdif_rgb_mode_config_t config = {
        .panelWidth    = APP_IMG_WIDTH,
        .panelHeight   = APP_IMG_HEIGHT,
        .hsw           = APP_HSW,
        .hfp           = APP_HFP,
        .hbp           = APP_HBP,
        .vsw           = APP_VSW,
        .vfp           = APP_VFP,
        .vbp           = APP_VBP,
        .polarityFlags = APP_POL_FLAGS,
        .bufferAddr    = (uint32_t) g_frameBuffer2,
        .pixelFormat   = kELCDIF_PixelFormatXRGB8888,
        .dataBus       = kELCDIF_DataBus16Bit,
    };

    ELCDIF_RgbModeInit(APP_ELCDIF, &config);

    EnableIRQ(LCDIF_IRQn);
}

/* -------------------------------------------------------------- */

static void APP_InitGpio()
{
    /* Define the init structure for the output pins pin */
    gpio_pin_config_t out_config = {
        kGPIO_DigitalOutput,
        1,
        kGPIO_NoIntmode
    };

    /* Init output J22-PIN7 */
    GPIO_PinInit(BOARD_J22P7_GPIO, BOARD_J22P7_PIN, &out_config);

    /* Init output J22-PIN8 */
    GPIO_PinInit(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, &out_config);
}

#if defined DRAW_GROWING_BOX

/* -------------------------------------------------------------- */

static void APP_DrawRectangle(uint8_t *fb,
                              int x1, int y1,
                              int x2, int y2,
                              uint32_t color)
{
    uint32_t *frameBuffer = (uint32_t *)fb;

    for (int y=y1; y <= y2; y++)
    {
        for (int x=x1; x <= x2; x++)
        {
            frameBuffer[ (y * APP_IMG_WIDTH) + x ] = color;
        }
    }
}

/* -------------------------------------------------------------- */

static void APP_FillFrameBuffer2(uint8_t *fb)
{
    const int boxWidth = (APP_IMG_WIDTH / 3);
	const int x1 = (APP_IMG_WIDTH / 2) - (boxWidth / 2);
    const int x2 = (APP_IMG_WIDTH / 2) + (boxWidth / 2);

    const int stepMax = 135;
    const int stepY = 2;
    static int stepDir = -1;
    static int stepN = 0;

    static int y1 = (APP_IMG_HEIGHT - 1);
    static int y2 = (APP_IMG_HEIGHT - 1);

    static uint32_t color = 0x00FF0000U;

    APP_DrawRectangle(fb,
                      x1, y1,
                      x2, y2,
                      color);

    if (++stepN == stepMax)
    {
        stepN = 0;
        if (stepDir < 0)
        {
        	y2 = y1 - stepY;
            stepDir = 1;
            color = 0x00000000U;
        }
        else
        {
            y1 = (APP_IMG_HEIGHT - 1);
            stepDir = -1;
            color = 0x00FF0000U;
        }
    }

    if (stepDir < 0)
    {
        y1 -= stepY;
    }
    else
    {
        y2 += stepY;
    }
}

#else // DRAW_GROWING_BOX

/* -------------------------------------------------------------- */

static void APP_FillFrameBuffer2(uint8_t *fb)
{
    uint32_t *frameBuffer = (uint32_t *)fb;

    // Background color
    const uint32_t bgColor = 0xFFFFFFFF; // White

    // Foreground color
    static uint8_t fgColorIndex = 0U;
    const uint32_t fgColorTable[] = {0x000000FFU,
                                     0x0000FF00U,
                                     0x0000FFFFU,
                                     0x00FF0000U,
                                     0x00FF00FFU,
                                     0x00FFFF00U,
                                     0x00000000U};
    const uint32_t fgColor = fgColorTable[fgColorIndex];

    // Position of the foreground rectangle
    const  uint16_t upperLeftX  = 1U;
    static uint16_t upperLeftY  = 0U;
    const  uint16_t lowerRightX = (APP_IMG_WIDTH - 2U) / 1U;
    static uint16_t lowerRightY = (APP_IMG_HEIGHT - 1U) / 4U;

    static int8_t incY = 1;

    // Change color in next frame or not
    static bool changeColor = false;

    // Background color
    for (int y=0; y < APP_IMG_HEIGHT; y++)
    {
        for (int x=0; x < APP_IMG_WIDTH; x++)
        {
            frameBuffer[ (y * APP_IMG_WIDTH) + x ] = bgColor;
        }
    }

    // Foreground color
    for (int y=upperLeftY; y < lowerRightY; y++)
    {
        for (int x=upperLeftX; x <= lowerRightX; x++)
        {
            frameBuffer[ (y * APP_IMG_WIDTH) + x ] = fgColor;
        }
    }

    // Update the format: color and rectangle position
    upperLeftY  += incY;
    lowerRightY += incY;
    changeColor = false;

    if (0U == upperLeftY)
    {
        incY        = 1;
        changeColor = true;
    }
    else if ((APP_IMG_HEIGHT - 1U) <= lowerRightY)
    {
        lowerRightY = (APP_IMG_HEIGHT - 1U);
        incY        = -1;
        changeColor = true;
    }

    if (changeColor)
    {
        fgColorIndex++;

        if (ARRAY_SIZE(fgColorTable) == fgColorIndex)
        {
            fgColorIndex = 0U;
        }
    }
}
#endif // DRAW_GROWING_BOX

/* -------------------------------------------------------------- */

int main(void)
{
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_InitSemcPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    APP_InitLcdifPixelClock();
    APP_InitLcd();
    APP_ELCDIF_Init();

    APP_InitGpio();

#if defined USE_TICK_LOAD
    // Set systick reload value to generate 1ms interrupt
    SysTick_Config(SystemCoreClock / 1000U);
#endif

    // Set J22-PIN7/PIN8
    GPIO_PinWrite(BOARD_J22P7_GPIO, BOARD_J22P7_PIN, 0U);
    GPIO_PinWrite(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, 0U);

    printf("evkbimxrt1050_test : %s, SystemCoreClock=%uMHz\r\n",
           __func__, SystemCoreClock/1000000);

    // Clear the double frame buffer
    memset(g_frameBuffer2, 0, 2 * FB_SIZE);

    // Prepare the first frame
    uint8_t fbIdxActive = 0;
    uint8_t fbIdxNext   = 1;

    APP_FillFrameBuffer2(g_frameBuffer2);
    ELCDIF_SetNextBufferAddr(APP_ELCDIF, (uint32_t)g_frameBuffer2);

    g_frameDone = false;

    // Start the display controller @40Hz, T=25ms
    ELCDIF_EnableInterrupts(APP_ELCDIF, kELCDIF_CurFrameDoneInterruptEnable);
    ELCDIF_RgbModeStart(APP_ELCDIF);

    while (1)
    {
#if defined USE_TICK_LOAD
        SysTick_DelayTicks(15); //Delay in [ms]
#endif
        // Prepare next frame when active frame is drawn by display controller
        uint8_t *fbAddr = &g_frameBuffer2[fbIdxNext * FB_SIZE];
        APP_FillFrameBuffer2(fbAddr);
        GPIO_PinWrite(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, 0U);

        // Give address of next frame to be drawn by display controller
        ELCDIF_SetNextBufferAddr(APP_ELCDIF, (uint32_t)fbAddr);

        // Wait for active frame to complete
        while (!g_frameDone) {}

        // Display controller is now in vertical blanking state
        GPIO_PinWrite(BOARD_J22P7_GPIO, BOARD_J22P7_PIN, 0U);
        GPIO_PinWrite(BOARD_J22P8_GPIO, BOARD_J22P8_PIN, 1U);
        g_frameDone = false;

        // Frame buffer swap
        uint8_t tmpIdx = fbIdxActive;
        fbIdxActive = fbIdxNext;
        fbIdxNext = tmpIdx;
    }
}
