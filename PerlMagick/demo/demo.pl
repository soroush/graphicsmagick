#!/usr/local/bin/perl
#
# Overall demo of the major PerlMagick methods.
#
use Image::Magick;

#
# Read model & smile image.
#
print "Read images...\n";
$null=Image::Magick->new;
$null->Set(size=>'70x70');
$x=$null->ReadImage('NULL:black');
warn "$x" if "$x";

$model=Image::Magick->new();
$x=$model->ReadImage('model.gif');
warn "$x" if "$x";
$model->Label('Magick');
$model->Set(bordercolor=>'black');
$model->Set(background=>'black');

$smile=Image::Magick->new;
$x=$smile->ReadImage('smile.gif');
warn "$x" if "$x";
$smile->Label('Smile');
$smile->Set(bordercolor=>'black');
#
# Create image stack.
#
print "Transform image...\n";
$images=Image::Magick->new();
$example=$null->Clone();
push(@$images,$example);
$example=$null->Clone();
push(@$images,$example);
$example=$null->Clone();
push(@$images,$example);
$example=$null->Clone();
push(@$images,$example);
$example=$null->Clone();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Add Noise');
$example->AddNoise("LaplacianNoise");
push(@$images,$example);

$example=$model->Clone();
$example->Label('Annotate');
$example->Annotate(text=>'Magick',geometry=>'+0+20',font=>'Generic.ttf',
  fill=>'gold',gravity=>'North',pointsize=>14);
push(@$images,$example);

$example=$model->Clone();
$example->Label('Blur');
$example->Blur('0.0x1.0');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Border');
$example->Border(geometry=>'6x6',color=>'gold');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Channel');
$example->Channel();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Charcoal');
$example->Charcoal();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Composite');
$example->Composite(image=>$smile,compose=>'over',geometry=>'+35+65');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Contrast');
$example->Contrast();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Convolve');
$example->Convolve([1, 1, 1, 1, 4, 1, 1, 1, 1]);
push(@$images,$example);

$example=$model->Clone();
$example->Label('Crop');
$example->Crop(geometry=>'80x80+25+50');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Despeckle');
$example->Despeckle();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Draw');
$example->Draw(fill=>'none',stroke=>'gold',primitive=>'circle',
  points=>'60,90 60,120',strokewidth=>2);
push(@$images,$example);

$example=$model->Clone();
$example->Label('Detect Edges');
$example->Edge();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Emboss');
$example->Emboss();

push(@$images,$example);
$example=$model->Clone();
$example->Label('Equalize');
$example->Equalize();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Explode');
$example->Set(background=>'#000000FF');
$example->Implode(-1);
push(@$images,$example);

$example=$model->Clone();
$example->Label('Flip');
$example->Flip();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Flop');
$example->Flop();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Frame');
$example->Frame();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Gamma');
$example->Gamma(1.6);
push(@$images,$example);

$example=$model->Clone();
$example->Label('Gaussian Blur');
$example->GaussianBlur('0.0x1.5');
push(@$images,$example);

$gradient=Image::Magick->new;
$gradient->Set(size=>'130x194');
$x=$gradient->ReadImage('gradient:#20a0ff-#ffff00');
warn "$x" if "$x";
$gradient->Label('Gradient');
push(@$images,$gradient);

$example=$model->Clone();
$example->Label('Grayscale');
$example->Quantize(colorspace=>'gray');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Implode');
$example->Implode(0.5);
push(@$images,$example);

$example=$model->Clone();
$example->Label('Median Filter');
$example->MedianFilter();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Modulate');
$example->Modulate(brightness=>110,saturation=>110,hue=>110);
push(@$images,$example);
$example=$model->Clone();

$example->Label('Monochrome');
$example->Quantize(colorspace=>'gray',colors=>2,dither=>'false');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Negate');
$example->Negate();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Normalize');
$example->Normalize();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Oil Paint');
$example->OilPaint();
push(@$images,$example);

$plasma=Image::Magick->new;
$plasma->Set(size=>'130x194');
$x=$plasma->ReadImage('plasma:fractal');
warn "$x" if "$x";
$plasma->Label('Plasma');
push(@$images,$plasma);

$example=$model->Clone();
$example->Label('Quantize');
$example->Quantize();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Raise');
$example->Raise('10x10');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Reduce Noise');
$example->ReduceNoise();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Resize');
$example->Resize('50%');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Roll');
$example->Roll(geometry=>'+20+10');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Rotate');
$example->Rotate(45);
$example->Transparent(color=>'black');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Scale');
$example->Scale('60%');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Segment');
$example->Segment();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Shade');
$example->Shade(geometry=>'30x30',color=>'false');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Sharpen');
$example->Sharpen('0.0x1.0');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Shave');
$example->Shave('10x10');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Shear');
$example->Shear('45x45');
$example->Transparent(color=>'black');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Spread');
$example->Spread();
push(@$images,$example);

$example=$model->Clone();
$example->Label('Solarize');
$example->Solarize();
push(@$images,$example);

$example=$model->Clone();
$example->Set(background=>'#000000FF');
$example->Label('Swirl');
$example->Swirl(90);
push(@$images,$example);

$example=$model->Clone();
$example->Label('Unsharp Mask');
$example->UnsharpMask('0.0x1.0');
push(@$images,$example);

$example=$model->Clone();
$example->Label('Wave');
$example->Set(background=>'#000000FF');
$example->Wave('25x150');
push(@$images,$example);
#
# Create image montage.
#
print "Montage image...\n";
$montage=$images->Montage(geometry=>'130x194+10+5>',gravity=>'Center',
  bordercolor=>'green',borderwidth=>1,tile=>'5x1000',compose=>'over',
  background=>'#ffffff',font=>'Generic.ttf',pointsize=>18,fill=>'#600',
  stroke=>'none');

$logo=Image::Magick->new();
$logo->Read('logo:');
$logo->Crop('461x455+98+0');
$logo->Zoom('45%');
$montage->Composite(image=>$logo,geometry=>'+245+0',compose=>'Over');

print "Write image...\n";
$montage->Set(matte=>'false');
$montage->Write('demo.jpg');
print "Display image...\n";
$montage->Write('win:');
