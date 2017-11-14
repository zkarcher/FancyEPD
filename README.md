# FancyEPD
Display fancy graphics on ePaper.

![Photo of ePaper display: Angel](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/angel_small.jpg) ![Photo of ePaper display: Angel #2](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/angel2_small.jpg)

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

## Supported Screens

##### Crystalfontz
* [CFAP122250A0-0213 — 2.13" \[112 × 208\]](https://www.crystalfontz.com/product/cfap122250a00213-epaper-display-122x250-eink)
* [CFAP128296C0-0290 — 2.9" \[128 × 296\]](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)
* [CFAP128296C0-0290 — 2.9" \[128 × 296\] black+red](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)

##### Pervasive Displays
* [E2215CS062 — 2.15" \[122 x 250\]](http://www.digikey.com/product-detail/en/pervasive-displays/E2215CS062/E2215CS062-ND/5975949)

// TODO: Add a matrix of supported features per screen

## Supported Boards

FancyEPD is known to work with these boards:
*  [kicad-teensy-epaper](https://github.com/pdp7/kicad-teensy-epaper) by [Drew Fustini](https://github.com/pdp7)
* ESP8266 (NodeMCU) thanks to [Gustavo Reynaga](https://github.com/hulkco)
* [Crystalfontz ePaper Development Kit](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)

## FAQ

##### Q: Can you fix the afterimages/ghosting?

A: The short answer: *No.*

Ghosting artifacts are intrinsic to ePaper technology. Unlike other displays (LCD, OLED, etc) the persistent nature of e-ink means that pixels are stateful; they have "history". When a pixel is updated, the final color is influenced by the previous color.

You can mitigate the ghosting artifacts (...*somewhat*...) by increasing the time that charges are held. Use the `setCustomTiming()` method. Be aware that you will *never* eliminate ghosting entirely.

Don't be sad. FancyEPD uses ghosting to your advantage: grayscale images are generated using several short updates. Each update is a blend of the new colors, and colors left behind from the previous update. Thus, a variety of shades between black and white can be generated. You're welcome!

##### Q: After a partial update, I see an afterimage, or the colors are inconsistent. Can you fix this?

A: The short answer: *No.*

(See the previous answer.)

You can mitigate color artifacts (...*somewhat*...) with certain update modes. Rather than using `k_update_no_blink` relentlessly, you will achieve better color reproduction if there's an occasional blinking refresh in the mix (like `k_update_quick_refresh`). The default update mode is `k_update_auto`, which does this automatically.

##### Q: It feels like putting images on ePaper is more art than science. Do you agree?

A: There's definitely an art to it! I'm still wrangling the black+red Crystalfontz display, trying to get nice-looking, rapid updates. Issues include: color "bleed" (pixels affect their neighbors); finicky timing; red color is slow to appear; black->red is basically impossible. The struggle is real.
