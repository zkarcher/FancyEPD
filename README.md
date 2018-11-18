# FancyEPD
Display fancy graphics on ePaper.

![Photo of ePaper display: Angel](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/angel_small.jpg) ![Photo of ePaper display: Angel #2](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/angel2_small.jpg)
![Photo of ePaper display: Black + Red](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/black_and_red.jpg) ![Photo of ePaper display: Black + Yellow](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/black_and_yellow.jpg)

# **A Note From the Developer**

[@zkarcher](https://twitter.com/zkarcher) 2018-11-17: As of today, **I have stopped development of FancyEPD.** I'm proud of everything I've accomplished, especially the impact this project has had on people's lives and creative work.

FancyEPD was started after I reverse-engineered how to change the e-Ink "waveforms" (the voltage patterns that are used to affect pixel colors). This was exciting, because it made grayscale *easy*! By sending the individual bits of an image as layers, and drawing each layer with very short waveforms, it's possible to use e-Ink's ghosting artifacts to gently influence each pixel, nudging it towards black or white ... and produce a range of grayscale colors.

At the time, I didn't realize that creating a "general-purpose" e-Ink library to support dozens of boards (and manufacturers' drivers, and fancy features) would be a long, tedious undertaking. As I add features, FancyEPD requires a huge amount of testing, and I don't have enough spare time for this. Some boards have features that are broken/unfinished.

Also, the physical properties of e-Ink are difficult to reconcile. I've tried creating short(er) refreshes on color displays, and except for a few use cases (big blobs of color, no fine details) I've basically failed. This problem sucked me in for too long (see: [XKCD nerd sniping](https://www.xkcd.com/356/)) and it may be unsolveable.

I still have some love for e-Ink displays. They're essentially an *analog* display technology, which rewards clever thinking and experimentation. If you're seeking a developer for a paid ePaper project, please contact me!

I feel like the world needs better, more reliable ePaper displays, with faster refresh times and better color accuracy. In the meantime, you're welcome to salvage whatever's in the FancyEPD repository. Happy coding!

-- Zach

![Photo of ePaper display: Mandelbrot](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/mandelbrot.jpg)

## Features

### Grayscale images.

```c
// Prepare your image in the image_gen.html tool, then:
epd.updateWithCompressedImage(image_data);
```

### Redraw without blinking the screen.

```c
epd.update(k_update_no_blink);
```

### Multiple refresh sequences.

```c
epd.update(k_update_builtin_refresh);	// Manufacturer's default. Lots of blinking.

epd.update(k_update_quick_refresh);	// One short blink, to restore image quality.

epd.update(k_update_no_blink);	// Redraw without blinking the screen!

epd.update(k_update_partial);	// Weaker than k_update_no_blink.
// Only applies charge to pixels which change.
// Other pixels may drift towards grey.

epd.update(k_update_auto);	// Trade-off between image quality and speed.
// Mostly uses _no_blink, occasionally performs
// a _quick_refresh to maintain image quality.
```

### Extends the [Adafruit GFX core library](https://github.com/adafruit/Adafruit-GFX-Library).

```c
// Clear the screen.
epd.clearBuffer(0x0);	// Set every pixel to white

// Get the display resolution.
int16_t width = epd.width();
int16_t height = epd.height();

// Draw concentric circles.
for (int16_t i = 1; i <= 20; i++) {
	int16_t radius = i * 2;
	epd.drawCircle(width / 2, height / 2, radius, 0xff);
}

// Print some text.
epd.print("Hello world!");

// Finally, tell the EPD to redraw:
epd.update();
```

### Rotation (screen orientation).

```c
epd.setRotation(0);	// 0 => default, 1 => 90°, etc.
```

### Border color.

```c
epd.setBorderColor(0x00);	// white
epd.setBorderColor(0xff);	// black
```

### Drag-and-drop image generation tool (with automatic compression).

Live demo: [image_gen.html tool](http://zacharcher.com/lab/FancyEPD/html/image_gen.html)

### Animation mode.

Faster updates, when small regions of the screen are changed.

```c
epd.setAnimationMode(true);
```

### Custom waveform timing.

```c
// args: [update type, normal image time, inverted image time (if appropriate).]

epd.setCustomTiming(k_update_quick_refresh, 50, 20);
```

### Hardware and software SPI support.

```c
// args: [model, chip select (CS), data/command (DC), register select (RS),
//        busy signal (BS), optional: SCLK, MOSI]

FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15);	// Hardware SPI
FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15, 13, 11);	// Software SPI
```

### Color panel support. *(experimental)*

Color displays (black+red, black+yellow) work reliably with the built-in refresh, which typically has a lot of blinking. Other refresh types (non-blinking, etc) are broken in various ways.

## Supported Screens

##### Crystalfontz
* [CFAP152152A0-0154 — 1.54" \[152 × 152\] black+red](https://www.crystalfontz.com/product/cfap152152a00154-epaper-square-eink-display)
* [CFAP152152B0-0154 — 1.54" \[152 × 152\] black+yellow](https://www.crystalfontz.com/product/cfap152152b00154-3-color-epaper-module)
* [CFAP122250A0-0213 — 2.13" \[112 × 208\]](https://www.crystalfontz.com/product/cfap122250a00213-epaper-display-122x250-eink)
* [CFAP104212D0-0213 — 2.13" \[104 × 212\] flexible](https://www.crystalfontz.com/product/cfap104212d00213-flexible-epaper-display)
* [CFAP128296C0-0290 — 2.9" \[128 × 296\]](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)
* [CFAP128296C0-0290 — 2.9" \[128 × 296\] black+red](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)

##### Pervasive Displays
* [E2215CS062 — 2.15" \[122 x 250\]](http://www.digikey.com/product-detail/en/pervasive-displays/E2215CS062/E2215CS062-ND/5975949)

// TODO: Add a matrix of supported features per screen

![Photo of ePaper display: Flexible Crystalfontz display](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/flexible.jpg)

## Supported Driver Boards

FancyEPD is known to work with these boards:
*  [kicad-teensy-epaper](https://github.com/pdp7/kicad-teensy-epaper) by [Drew Fustini](https://github.com/pdp7)
* ESP8266 (NodeMCU) thanks to [Gustavo Reynaga](https://github.com/hulkco)
* [Crystalfontz ePaper Development Kit](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)
* [Crystalfontz new ePaper Adapter Board](https://www.crystalfontz.com/product/cfa10084-epaper-adapter-board)

## FAQ

##### Q: Can you fix the afterimages/ghosting?

A: The short answer: *No.*

Ghosting artifacts are intrinsic to e-Ink's ePaper. Unlike other displays (LCD, OLED, etc) the persistent nature of e-Ink means that pixels are stateful; they have "history". When a pixel is updated, the final color is influenced by the previous color.

You can mitigate the ghosting artifacts (...*somewhat*...) by increasing the time that charges are held. Use the `setCustomTiming()` method. Be aware that you will *never* eliminate ghosting entirely.

Don't be sad. FancyEPD uses ghosting to your advantage: grayscale images are generated using several short updates. Each update is a blend of the new colors, and colors left behind from the previous update. Thus, a variety of shades between black and white can be generated. You're welcome!

##### Q: After a partial update, I see an afterimage, or the colors are inconsistent. Can you fix this?

A: The short answer: *No.* (See the previous question.)

You can mitigate color artifacts (...*somewhat*...) with certain update modes. Relentlessy hammering the display with endless `k_update_no_blink` updates will produce a lot of ghosting, and ugly colors. You'll achieve better color reproduction if there's an occasional blinking refresh in the mix (like `k_update_quick_refresh`). The default update mode is `k_update_auto`, which does this automatically.

##### Q: It feels like putting images on ePaper is more art than science. Do you agree?

A: There's definitely an art to it! I'm still wrangling the color displays, trying to get nice-looking, rapid updates. Issues include: color "bleed" (pixels affect their neighbors); finicky timing; red color is slow to appear; black->red transition without blinking may be literally impossible. The struggle is real.
