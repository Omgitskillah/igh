@ECHO OFF
rem $ curl -X PUT "https://api.particle.io/v1/devices/0123456789abcdef01234567?access_token=1234" \
rem       -F file=@my-firmware-app.bin \
rem       -F file_type=binary
if "%1"=="" (
    ECHO Invalid Access Token...
) else (
    if "%2"=="" (
        ECHO Invalid Boron ID...
    ) else (
        if "%3"=="" (
            ECHO invalid image...
        ) else (
            ECHO UPDATING %2 to %3, Access Key: %1
            curl -X PUT "https://api.particle.io/v1/devices/%2?access_token=%1" -F file=@%3 -F file_type=binary
        )
    )
)