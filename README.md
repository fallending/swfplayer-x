# swfplayer-x
Try to run swf player on iOS

## Reference

1. [gameswf](http://tulrich.com/geekstuff/gameswf.html), 可以找到macOS、win32版本，不过win32的我还没编译通过，优先macOS，如[链接](https://sourceforge.net/projects/tu-testbed/files/demos/gameswf-2009-08-08/), 该页面 igameswf.tar.gz 是macOS版本；gameswf-2009-08-08.zip 是win32版本。

2. [dirmich/MonkSWF](https://github.com/dirmich/MonkSWF) + Cocos2D

3. [musictheory/SwiffCore](https://github.com/musictheory/SwiffCore)

4. [chenee/cocos2dx-swf](https://github.com/chenee/cocos2dx-swf), 相比1中的gameswf，这里作者将OpenGLES 升级到 2.0了！赞！

## Bugfix

1. [gameswf can't load swf file under 64 bit ios](http://stackoverflow.com/questions/30887855/gameswf-cant-load-swf-file-under-64-bit-ios)

2. [sagesse-cn 's comments](https://sourceforge.net/projects/tu-testbed/?source=typ_redirect)
```
very good, but can't support 64bit on Xcode 6 I have found the problem unsigned int hash_value = (unsigned int) compute_hash(key); Modified to: size_t  hash_value = (size_t) compute_hash(key); entry(const T& key, const U& value, int next_in_chain, int hash_value) Modified to: entry(const T& key, const U& value, int next_in_chain, size_t hash_value)
```

3. [Playing Around With Flash on iOS](http://www.paradeofrain.com/2011/05/20/playing-around-with-flash-on-ios/)

4. swf opengles 1.0 迁移到 2.0
    * [OpenGLES 1.1 相对于OpenGL2.1的删减-材质部分](http://blog.csdn.net/n5/article/details/5596310)
        - Color Material
        OpenGLES 1.1只支持使用glEnable/glDisable开启关闭，如果开启，必然是diffuse & ambient同时trace, 而OpenGL2.1可以使用glColorMaterial(GLenum face, GLenum mode)指定正反面的color material模式，即使用ambient, diffuse, specular,emission,还是ambient & diffuse trace.
        - Blend 删减比较多
        -- Blend Op: 
        OpenGL2.1支持设置blend operation,如add, subtract, reverse subtract, min, max，而OpenGLES1.1不支持设置，只能使用add
        -- Blend Factor的设定: 
        OpenGL2.1可以分别设定RGB和Alpha的Factor，而ES1.1不支持，2.1还支持分别设置RGB和alpha的Op，ES1.1当然更不支持了
        -- Blend Factor:
        ES1.1不支持 GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA
    * [OpenGLes1_0与OpenGLes2_0在开发过程中的异同](http://www.docin.com/p-1235896646.html)

## Others

1. [matthiaskramm/swftools](https://github.com/matthiaskramm/swftools)

2. [phylake/libswf](https://github.com/phylake/libswf)