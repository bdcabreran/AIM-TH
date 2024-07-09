
file(GLOB srcs
        "*.c"
        "APIs/HTTP_SERVER/*.c"
        "APIs/WIFI_API/*.c"
        "APIs/SPIFFS/*c"
        "APIs/NVS/*.c"
        # "local_components/lvgl_app/*.c"
        "local_components/buttons/*.c"
        "local_components/light/*.c"
        "local_components/led/*.c"
        "local_components/aim/*.c"
        "local_components/ui/*.c"
        "local_components/ui/img/*.c"
        "local_components/ui/screen/wifi_screen/*.c"
        "local_components/ui/screen/main_screen/*.c"
        "local_components/event_router/*.c"
        )
        
set(dirs
        "APIs/HTTP_SERVER"
        "APIs/WIFI_API"
        "APIs/SPIFFS"
        "APIs/NVS"
        # "local_components/lvgl_app"
        "local_components/buttons"
        "local_components/light"
        "local_components/led"
        "local_components/aim"
        "local_components/ui"
        "local_components/ui/screen/wifi_screen"
        "local_components/event_router"
        )


# # not needed
# set(components
#         "lvgl_esp32_drivers"
#         "lvgl_touch"
#         "lvgl_tft"
#         "lvgl"
# )

set(files
        "../doc/web/style.css"
        "../doc/web/app.js"
        "../doc/web/favicon.ico"
        "../doc/web/index.html"
        "../doc/web/upload_script.html"
        "../doc/web/jquery-3.3.1.min.js"
)

idf_component_register(
        SRCS ${srcs}
        INCLUDE_DIRS ${dirs}
        # REQUIRES ${components} 
        EMBED_FILES ${files}
        )

# idf_component_register(SRCS "main.c"
#                        INCLUDE_DIRS "")
