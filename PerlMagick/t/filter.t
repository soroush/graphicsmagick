#!/usr/local/bin/perl
#
# Test image filter.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..43\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

testFilterSignature('input.miff', 'Blur',
  q/0.0x1.0/,
  '66c8403de767bb48540fcf19e50ae61bb9346b39f744399fade43e28795c181d',
  '86429e2662d7083733e0b0d75497da57ba212649859ac261a72f1245b4d220d2');

++$test;
testFilterSignature('input.miff', 'Border',
  q/10/,
  '1308b50682539ba231501d336c0f688372becf093fab928c131c0c8e9dfbed87');

++$test;
testFilterSignature('input.miff', 'Channel',
  q/Red/,
  '76b07c5ff3de0530bc6fcba511f1391cef261eaf4b0f9ee1e3859c7466d017e7');

++$test;
testFilterSignature('input.miff', 'Chop',
  q/10x10!/,
  '0cee3408e6e3255317340bde1ebe026359d58ec3989ecf315df34dd52925beca');

++$test;
testFilterSignature('input.miff', 'ColorFloodfill',
  q/+25+45/,
  '9767e3a22a9cdafe39c448903aea84e68881d39b250b4be921e3f3ebec53f9ee');

++$test;
testFilterSignature('input.miff', 'Colorize',
  q/red/,
  '767b270256d40ccec025494072c070256147a7046b464327ebea6f7934b96121');

++$test;
testFilterSignature('input.miff', 'Contrast',
  q/1/,
  'd22f5aa7610c7527355e9805aac6d3e0125b8b4d62cdb7a1a4e5bf5359c83e11',
  'f780476c04c823dc288b7cda85e8cd26bb97353323978b11ff7081a96209d1f2');

++$test;
testFilterSignature('input.miff', 'Convolve',
  [1, 2, 1, 2, 4, 2, 1, 2, 1],
  '6afcd7320fa3e02d0927dbde488f2bceca021d7ab06fcae6a578974433b5ba0e',
  'b7cf64547b875a5a2abe35734822d088d02390707dc79438cc8e24f877d94199');

++$test;
testFilterSignature('input.miff', 'Crop',
  q/10x10!/,
  'b66e37b497d4e7275b04fc8f0d0539aeca9a73c187ae239352c7abec44ad2fe9');

++$test;
testFilterSignature('input.miff', 'Despeckle',
  q/ /,
  '9e7f20a6f5a36cbc7809c4ef7e748583fad81c6718f5af799210666b76016368',
  'd3c153d8eed545e70371f82d5398c0a9649a90eff964c42ed94074586046438d');

++$test;
testFilterSignature('input.miff', 'Edge',
  q/3/,
  '8245bada67e7be6509c02ec6f8372a8c2eb0c04bdc6221ff839ac06e770b9bf2',
  'a062b84151d5b78a42d641f20f41e3b6cb148b79de3b2349d2ac722b87d3700c');

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Emboss.miff', 'Emboss',
  q//, 0, 0);

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Equalize.miff', 'Equalize',
  q//, 0, 0);

++$test;
testFilterSignature('input.miff', 'Flip',
  q/ /,
  '092fa2bb0e2c1826a7b0ac9ea42a788684230623fa56bf0c4c2c9e1d71de8bf1');

++$test;
testFilterSignature('input.miff', 'Flop',
  q/ /,
  'f847ea7cc9b78677c083df656978081827c7bfb532a4c2d5fd3eed19e8b73a3a');

++$test;
testFilterSignature('input.miff', 'Frame',
  q/10x10/,
  'b4956b654102b9af96956017f94c57d214110d91cb7be995e5ce19d29b0a3eba', 
  '0469d077e745fe177f42abe4383cd9fd6f3aba5a1fa24ba67268390f540daeff');

++$test;
testFilterSignature('input.miff', 'Gamma',
  q/2.2/,
  '500b17abf69d4c9e11e8f6a9c245a3a021ba935731f9d3d477ac07d9f4a6b197',
  'a643f8c1998c4d4e643e6de69b488d9ea349d9def527481a511ff7839c523651');

++$test;
testFilterSignature('input.miff', 'Implode',
  q/0.5/,
  'dd31bb88ac09b610724833bf0226391f62e57b1768ca171659c0af9a7b6e6a6b',
  'a70c95a3ea1e65073928ab1f9713a8fff7da8b93ea7d013ea1208707c635429f');

++$test;
testFilterSignature('input.miff', 'Magnify',
  q/ /,
  'f22df13ea461203e4b88e5ed608dc1f8905f18e3580bf934a30da8e3f03d3fb2',
  '207fe33176b222d1d1c543141fbe292ff20372a3727fd5fd576d172e9cddf95e');

++$test;
testFilterSignature('input.miff', 'MatteFloodfill',
  q/+25+45/,
  '6d18a2b8e149727c853c9f2e02f5807ca732ed15f5dbc03f45314d2fab95a631',
  '2c9e13828667822c69a403e789d1783b7d2d875527c9e22c1481e1acf66d889b');

++$test;
testFilterSignature('input.miff', 'Minify',
  q/ /,
  'f8b5694b96ad47e606927399c7613f5625376ecb79a676d9554907dcb20b61aa',
  '961a24c814039bb8ba1f7eadbd44e81c368145de7d5c3633a1095c26bcee369e');

++$test;
testFilterSignature('input.miff', 'Modulate',
  q/60/,
  '4a9c88f7ffaa8937920235af65d4b7d38d70d72315e4868a9068c3b1d0cbb308',
  '5d2a14f4ebece1c43e185ee9cefc05f6942f830d1a62b1a70de982da9627eda5');

++$test;
testFilterSignature('input.miff', 'Negate',
  q/False/,
  '13d2a0623ca4eefa189d9db65896ed0b3af21c7a61b3d34a1644e9fb151daef6');

++$test;
testFilterSignature('input.miff', 'Normalize',
  q/ /,
  'b1bcbe33aaeac67d8a9c8f43142fffb2895c83a4c923ff610e96ad595a75d4de',
  '48aca353af569e222be8a13a29ebaa05b9ef316a5464958a50d74d9618ab255c');

++$test;
testFilterSignature('input.miff', 'OilPaint',
  q/3/,
  'b9cc4ce7b365d8ddb2a4bc3803b810e2a8b6fdcd267670cf7a9b089fc53fb4a5',
  'a0df026736c29469614cb08e4fe9cfdec62a1dc7999db80a310fbcf554d60d03');

++$test;
testFilterSignature('input.miff', 'Opaque',
  q/#fefefd/,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

++$test;
testFilterSignature('input.miff', 'Quantize',
  q/128/,
  '2cfd3e729a57d13bef868d525d909d806df6b5ff595b6d5c88bd172f0eaef1e7',
  '8b93248e8d83be9caae3f0ff78a47bd85cc63372c18846943df0778de96a630c');

++$test;
testFilterSignature('input.miff', 'Raise',
  q/10x10/,
  'b31cae257270e0a9ab61181d79c57f2433e465a43f1c75fc70f1398473419c78',
  '3dd26d98684455a4d6de122ca1040ae44f646f64e0d67cc655e32977f6d2b8bf');

++$test;
testFilterSignature('input.miff', 'Resize',
  q/50%/,
  '034a25c1de9337e993d64629478c0f7826653cf8408b95511e98da17c8b16f92',
  '9d54ceadb2dd6b36eef5cd9e3c4f2015d277f93bf2205927ba41f449915b6edf');

++$test;
testFilterSignature('input.miff', 'Roll',
  q/+10+10/,
  '7d06e76bc4dbc8a1c5908dde113fe2f89644e6b9272b67575c648e97453f1a1d');

++$test;
testFilterSignature('input.miff', 'Rotate',
  q/10/,
  '677cccb5efad4b4188825c5e5d483a643bc764d07437e55f4734769cf17b1918',
  '138c2338b6216e981f8b0a95a67d60d58d8d6e98599e2efbba79d71c4a473df3');

++$test;
testFilterSignature('input.miff', 'Sample',
  q/50%/,
  '73662fad184d63125ab5b41543be900b016afaeef20cfc23d7d657f53d2e2c03');

++$test;
testFilterSignature('input.miff', 'Scale',
  q/50%/,
  '1e41fc85f160baa8cce4436ddf64e64e549b1e1bf941c75bc2860c6d1352022f',
  'ff2af316c85f4d38a707e974525def24ed8b9c25eaf247c960d8d923575f1add');

++$test;
testFilterCompare('input.miff', q//, 'reference/filter/Shade.miff', 'Shade',
  q//, 3.3e-06, 3.06e-05);

++$test;
testFilterSignature('input.miff', 'Segment',
  q/1/,
  'dfddaf9947c009961addec474d86d4142f418d20ff9de2460e8bd4928516aff3');

++$test;
testFilterSignature('input.miff', 'Sharpen',
  q/0.0x1.0/,
  '80d503a182ba0b6a88e840c5cf5912303cf7765db91cb9ed886d9dbc5f3102f8',
  'f272b45836f45fb18bde63b4c3936dc1c5e84b24e84ce1daa302263b4ae5370e');

++$test;
testFilterSignature('input.miff', 'Shave',
  q/10x10/,
  '6f8749013dbe04020176a6e1d4a8a6783c2651fd0ef05cd9eb2dd66d38fa916f');

++$test;
testFilterSignature('input.miff', 'Shear',
  q/10/,
  '7dd7b15ed83be0a3898ce1c06ef6ff35ace29f4beefd09f51fba88bc5b929389',
  'b40146de3357c0c156c3b23398b294b125aca4c832ca83ee5e737c5ab2df403d');

++$test;
testFilterSignature('input.miff', 'Solarize',
  q/60/,
  '7aa9589246439e23b6a4504cae40b9e32b0e5ee34a6c3ede14ffa958d39806b0',
  '13d2a0623ca4eefa189d9db65896ed0b3af21c7a61b3d34a1644e9fb151daef6');

++$test;
testFilterSignature('input.miff', 'Swirl',
  q/60/,
  'e9e50b5e2c8abb68ecda0550fb512c237df3c864be979198fbcdf72cbc491595',
  '596d330a03d4270fcd20e58dc7e1ccee967907c897ceda18cdd8a527f805469c');

++$test;
testFilterSignature('input.miff', 'Threshold',
  ($MaxRGB+1)/2,
  '36c9b367b84035869df118d2fc61cbd7060d419d8626c4147955c712bcab15c0',
  '40db28af0162a4abd3ee8a1094591cd7eb07463464d7c235e4c2f2ae40aa3686');

++$test;
testFilterSignature('input.miff', 'Trim',
  q/0/,
  '8b19185a62241bd7b79ecf3f619711f4ebbedd73eaeca0366f05778762b6614f');

++$test;
testFilterSignature('input.miff', 'Wave',
  q/25x157/,
  '7e193ce861b72cb9e5e405cc1aa882098060fbe35927c8cb5fab294ab0b8a0cf',
  'a45df92ef008a3e5053f8716efdebdc9870d541391e9ae20ff7f4ca186c1483a');

++$test;

1;
