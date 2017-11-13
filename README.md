# FancyEPD
Display fancy graphics on ePaper.

![Photo of ePaper display: Angel](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/angel_small.jpg) ![Photo of ePaper display: Angel #2](https://raw.githubusercontent.com/zkarcher/FancyEPD/master/images/angel2_small.jpg)

## Features

### Grayscale images

```c
// Prepare your image in the image_gen.html tool, then:
epd.updateWithCompressedImage(image_data);
```

### Partial updates (no screen blinking)

```c
epd.update(k_update_no_blink);	// strong update, with no blinking!
```

### Multiple refresh sequences to choose from

```c
epd.update(k_update_builtin_refresh);	// manufacturer's default. Lots of blinking.

epd.update(k_update_quick_refresh);	// One short blink, to refresh display quality.

epd.update(k_update_no_blink);	// strong update with no blinking!

epd.update(k_update_partial);	// weaker than _no_blink. Only charges pixels which change. Other pixels may drift towards grey.

epd.update(k_update_auto);	// Recommended. See the FAQ below!
```

### Border color

```c
epd.setBorderColor(0x00);	// white
epd.setBorderColor(0xff);	// black
```

### Rotation (screen orientation)

```c
epd.setRotation(0);	// 0 => default, 1 => 90°, etc.
```

### Drag-and-drop image generation tool, with automatic compression

Live demo: [image_gen.html tool](http://zacharcher.com/lab/FancyEPD/html/image_gen.html)

### Animation mode

```c
// Faster updates, when small regions of the screen are redrawn.
epd.setAnimationMode(true);
```

### Custom waveform timing

```c
// args: [update type, normal time, inverted refresh time (if appropriate)]
epd.setCustomTiming(k_update_quick_refresh, 50, 20);
```

### Hardware and software SPI support

```c
// args: [model, chip select (CS), data/command (DC), register select (RS), busy signal (BS), optional: SCLK, MOSI]

FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15);	// hardware SPI
FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15, 13, 11);	// software SPI
```

## Supported Screens

##### Crystalfontz
* [CFAP122250A0-0213 — 2.13" \[112 × 208\]](https://www.crystalfontz.com/product/cfap122250a00213-epaper-display-122x250-eink)
* [CFAP128296C0-0290 — 2.9" \[128 × 296\]](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)
* [CFAP128296C0-0290 — 2.9" \[128 × 296\]](https://www.crystalfontz.com/product/cfap128296c00290-128x296-epaper-display-eink)

##### Pervasive Displays
* [E2215CS062 — 2.15" \[122 x 250\]](http://www.digikey.com/product-detail/en/pervasive-displays/E2215CS062/E2215CS062-ND/5975949)

// TODO: Add a matrix of supported features per screen

## Supported Boards (confirmed working)
*  [kicad-teensy-epaper](https://github.com/pdp7/kicad-teensy-epaper) by [Drew Fustini](https://github.com/pdp7)
* ESP8266 (NodeMCU) thanks to [Gustavo Reynaga](https://github.com/hulkco)
* Teensy
* Seeeduino

## FAQ

##### Q: Can you fix the afterimages/ghosting?

A: The short answer: *No.*

Ghosting artifacts are intrinsic to e-paper technology. Unlike other displays (LCD, OLED, etc) the persistent nature of e-ink means that pixels have "history". When a pixel is updated, the resultant color is affected by the color present before the update occurred.

You can mitigate the ghosting artifacts (...*somewhat*...) by increasing the time that charges are held. Use the `setCustomTiming()` method. Be aware that you will *never* eliminate ghosting entirely.

Don't cry. FancyEPD uses ghosting to your advantage: grayscale images are generated using several short updates. Each update is a blend of the new colors, and the colors left behind from the previous update. Thus, a variety of shades between black and white can be created. You're welcome!

##### Q: After a partial update, there's an afterimage, or the colors are inconsistent. Can you fix this?

A: The short answer: *No.*

(See the previous answer.)

You can mitigate color artifacts (...*somewhat*...) by using a different update method. Rather than using `k_update_no_blink` relentlessly, you will achieve better color reproduction if there's an occasional blinking refresh in the mix (like `k_update_quick_refresh`). The default update mode is `k_update_auto`, which does exactly this, automatically.

##### Q: Why does it feel like putting images on e-paper is more art than science?

A: There's definitely an art to it! I'm still wrangling the black+red Crystalfontz display, trying to get nice-looking, rapid updates. Issues include: color "bleed" (pixels affect their neighbors); finicky timing; red color takes a long time to appear; black->red is basically impossible. The struggle is real.
