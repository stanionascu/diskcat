# diskcat

Diskcat is an utility that opens and reads the content of DVD images into stdout for reprodcessing.
You can convenienetly pipe the output into FFmpeg or FFprobe, to identify the streams or remux into another format.

## Compiling
```
$ cmake --build .
```

## Reading a DVD image or folder

### Identifying the streams in a DVD disc image or folder
```
$ diskcat /archive/SOME_FILE.iso | ffprobe -probesize 20M -analyzeduration 20M -i - -show_streams -loglevel error -print_format json
{
    "streams": [
        {
            "index": 0,
            "codec_name": "dvd_nav_packet",
            "codec_long_name": "DVD Nav packet",
            "codec_type": "data",
            "codec_tag_string": "[0][0][0][0]",
            "codec_tag": "0x0000",
            "id": "0x1bf",
            "r_frame_rate": "0/0",
            "avg_frame_rate": "0/0",
            "time_base": "1/90000",
            "start_pts": 18228,
            "start_time": "0.202533",
            "disposition": {
                "default": 0,
                "dub": 0,
                "original": 0,
                "comment": 0,
                "lyrics": 0,
                "karaoke": 0,
                "forced": 0,
                "hearing_impaired": 0,
                "visual_impaired": 0,
                "clean_effects": 0,
                "attached_pic": 0,
                "timed_thumbnails": 0
            }
        },
        {
            "index": 1,
            "codec_name": "mpeg2video",
            "codec_long_name": "MPEG-2 video",
            "profile": "Main",
            "codec_type": "video",
            "codec_time_base": "1/25",
            "codec_tag_string": "[0][0][0][0]",
            "codec_tag": "0x0000",
            "width": 720,
            "height": 576,
            "coded_width": 0,
            "coded_height": 0,
            "has_b_frames": 1,
            "sample_aspect_ratio": "64:45",
            "display_aspect_ratio": "16:9",
            "pix_fmt": "yuv420p",
            "level": 8,
            "color_range": "tv",
            "color_space": "bt470bg",
            "color_transfer": "bt470bg",
            "color_primaries": "bt470bg",
            "chroma_location": "left",
            "field_order": "tt",
            "refs": 1,
            "id": "0x1e0",
            "r_frame_rate": "25/1",
            "avg_frame_rate": "25/1",
            "time_base": "1/90000",
            "start_pts": 18228,
            "start_time": "0.202533",
            "bit_rate": "6500000",
            "disposition": {
                "default": 0,
                "dub": 0,
                "original": 0,
                "comment": 0,
                "lyrics": 0,
                "karaoke": 0,
                "forced": 0,
                "hearing_impaired": 0,
                "visual_impaired": 0,
                "clean_effects": 0,
                "attached_pic": 0,
                "timed_thumbnails": 0
            }
        },
        {
            "index": 2,
            "codec_name": "ac3",
            "codec_long_name": "ATSC A/52A (AC-3)",
            "codec_type": "audio",
            "codec_time_base": "1/48000",
            "codec_tag_string": "[0][0][0][0]",
            "codec_tag": "0x0000",
            "sample_fmt": "fltp",
            "sample_rate": "48000",
            "channels": 6,
            "channel_layout": "5.1(side)",
            "bits_per_sample": 0,
            "dmix_mode": "-1",
            "ltrt_cmixlev": "-1.000000",
            "ltrt_surmixlev": "-1.000000",
            "loro_cmixlev": "-1.000000",
            "loro_surmixlev": "-1.000000",
            "id": "0x80",
            "r_frame_rate": "0/0",
            "avg_frame_rate": "0/0",
            "time_base": "1/90000",
            "start_pts": 18228,
            "start_time": "0.202533",
            "bit_rate": "448000",
            "disposition": {
                "default": 0,
                "dub": 0,
                "original": 0,
                "comment": 0,
                "lyrics": 0,
                "karaoke": 0,
                "forced": 0,
                "hearing_impaired": 0,
                "visual_impaired": 0,
                "clean_effects": 0,
                "attached_pic": 0,
                "timed_thumbnails": 0
            }
        },
        {
...
        {
            "index": 13,
            "codec_name": "dvd_subtitle",
            "codec_long_name": "DVD subtitles",
            "codec_type": "subtitle",
            "codec_time_base": "0/1",
            "codec_tag_string": "[0][0][0][0]",
            "codec_tag": "0x0000",
            "id": "0x29",
            "r_frame_rate": "0/0",
            "avg_frame_rate": "0/0",
            "time_base": "1/90000",
            "start_pts": 1674228,
            "start_time": "18.602533",
            "disposition": {
                "default": 0,
                "dub": 0,
                "original": 0,
                "comment": 0,
                "lyrics": 0,
                "karaoke": 0,
                "forced": 0,
                "hearing_impaired": 0,
                "visual_impaired": 0,
                "clean_effects": 0,
                "attached_pic": 0,
                "timed_thumbnails": 0
            }
        }
    ]
}
```

### Remuxing DVD to MKV via FFmpeg

It may be necessary to add `+genpts`, to generate the presentation timestamps from content. Not all formats allow muxing without them.

```
$ diskcat /archive/SOME_FILE.iso | ffmpeg -fflags +genpts -analyzeduration 20M -probesize 20M -i - -map 0 -map -0:0 -codec copy OUTPUT.mkv
...
I0115 18:47:07.230089   12360 diskcat.cc:62] Title playback finished.
frame=167697 fps=2035 q=-1.0 Lsize= 5879688kB time=01:51:47.96 bitrate=7180.5kbits/s speed=81.4x
```
