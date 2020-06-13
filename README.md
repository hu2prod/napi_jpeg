# napi_jpeg
decode jpeg

    mod = require("napi_jpeg")
    fs = require("fs")
    
    src = fs.readFileSync("test/input.jpg")
    dst = Buffer.alloc(4*1920*1080)
    [size_x, size_y, channel_count] = mod.jpeg_decode_rgba(src, dst)

## Thanks
inspired by zhangyuanwei/node-images
