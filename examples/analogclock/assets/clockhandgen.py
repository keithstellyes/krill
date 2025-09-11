#!/usr/bin/env python3
from datetime import datetime
from PIL import Image, ImageDraw, ImageColor

reference_bg = Image.open('clockface.png').convert('RGBA')
bgw, bgh = reference_bg.size

print(f'Background clock face asset: {bgw}x{bgh}')

est_radius = min(bgw, bgh) / 2
print('estimated clock radius:', est_radius)

mins_width = .02
mins_len = bgh*.15

mins = Image.new('RGBA', size=(bgw, bgh), color=(0, 0, 0, 0))
draw = ImageDraw.Draw(mins)
draw.rectangle((int(bgw/2-bgw*(mins_width/2)), int(mins_len), int(bgw/2+bgw*(mins_width/2)), int(bgh/2)), fill='black')
mins.save('minsshand.png')

hrs_width = .02
hrs_len = bgh*.3
hours = Image.new('RGBA', size=(bgw, bgh), color=(0, 0, 0, 0))
draw = ImageDraw.Draw(hours)
draw.rectangle((int(bgw/2-bgw*(hrs_width/2)), hrs_len, int(bgw/2+bgw*(hrs_width/2)), int(bgh/2)), fill='black')
hours.save('hourshand.png')

# generate example

exhours = hours.rotate(300)
exmins = mins.rotate(20)

example = Image.alpha_composite(Image.alpha_composite(reference_bg, exhours), exmins).save('example.png')

hnow = datetime.now().hour % 12
mnow = datetime.now().minute

hours = hours.rotate(-360 * (hnow / 12))
mins = mins.rotate(-360 * (mnow / 60))
print(mnow)
now = Image.alpha_composite(Image.alpha_composite(reference_bg, hours), mins).save('now.png')

