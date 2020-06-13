# napi_jpeg
decode jpeg

    mod = require("napi_jpeg")
    fs = require("fs")
    
    src = fs.readFileSync("test/input.jpg")
    res = mod.jpeg_decode_size(src)
    console.log(res)
    size_x = res[0]
    size_y = res[1]
    dst = Buffer.alloc(4*size_x*size_y)
    mod.jpeg_decode_rgba(src, dst)

## Thanks
inspired by zhangyuanwei/node-images
