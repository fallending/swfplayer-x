# swfplayer-x
Try to run swf player on iOS

## Reference

1. [gameswf](http://tulrich.com/geekstuff/gameswf.html), 可以找到macOS、win32版本，不过win32的我还没编译通过，优先macOS，如[链接](https://sourceforge.net/projects/tu-testbed/files/demos/gameswf-2009-08-08/), 该页面 igameswf.tar.gz 是macOS版本；gameswf-2009-08-08.zip 是win32版本。

2. [dirmich/MonkSWF](https://github.com/dirmich/MonkSWF) + Cocos2D

3. [musictheory/SwiffCore](https://github.com/musictheory/SwiffCore)

## Bugfix

1. [gameswf can't load swf file under 64 bit ios](http://stackoverflow.com/questions/30887855/gameswf-cant-load-swf-file-under-64-bit-ios)

2. [sagesse-cn 's comments](https://sourceforge.net/projects/tu-testbed/?source=typ_redirect)
```
very good, but can't support 64bit on Xcode 6 I have found the problem unsigned int hash_value = (unsigned int) compute_hash(key); Modified to: size_t  hash_value = (size_t) compute_hash(key); entry(const T& key, const U& value, int next_in_chain, int hash_value) Modified to: entry(const T& key, const U& value, int next_in_chain, size_t hash_value)
```

3. [Playing Around With Flash on iOS](http://www.paradeofrain.com/2011/05/20/playing-around-with-flash-on-ios/)

## Others

1. [matthiaskramm/swftools](https://github.com/matthiaskramm/swftools)

2. [phylake/libswf](https://github.com/phylake/libswf)