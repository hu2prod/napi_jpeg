# napi_jpeg
## install requirements

    apt-get install -y libjpeg-turbo8-dev

## use
decode jpeg

    mod = require("napi_jpeg")
    fs = require("fs")
    
    src = fs.readFileSync("test/input.jpg")
    res = mod.jpeg_decode_size(src)
    console.log(res)
    size_x = res[0]
    size_y = res[1]
    dst_rgba = Buffer.alloc(4*size_x*size_y)
    mod.jpeg_decode_rgba(src, dst_rgba)
    dst_rgb = Buffer.alloc(3*size_x*size_y)
    mod.jpeg_decode_rgb(src, dst_rgb)
    
    
    // encode API is really strange (but for purpose)
    // we want reuse as much buffers as possible (less allocs possible)
    jpeg_dst = Buffer.alloc(4*1920*1080)
    res = mod.jpeg_encode_rgba dst_rgba, size_x, size_y, jpeg_dst, target_buffer_desired_offset = 0, quality = 100
    start   = res[0]
    length  = res[1]
    buf     = res[2]
    // now we have 2 different situations
    //  
    //  1. encoded_length + target_buffer_desired_offset < jpeg_dst.length
    //  then we can reuse buffer, so we return buf == jpeg_dst, start == target_buffer_desired_offset, length == encoded_length
    //  2. not enough space to write
    //  then we need allocate new buffer, buf != jpeg_dst, start == 0, length == encoded_length
    
    // target_buffer_desired_offset is needed if we want use 1 big buffer for multiple images, so we can use space more effectively (theoretically for some applications)
    
    // mod.jpeg_encode_rgb is also available

## Thanks
inspired by zhangyuanwei/node-images
