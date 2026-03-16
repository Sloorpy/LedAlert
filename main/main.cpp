#include <cstdint>
#include <string>

#include <driver/gpio.h>

#include "SystemInitializer.hpp"
#include "UserData.h"
#include "AlertsManager.hpp"
#include "LedManager.hpp"
#include "AlarmController.hpp"
#include "EspException.hpp"
#include "IRException.hpp"
#include "Exception.hpp"

extern "C" void app_main(void) {
    std::unique_ptr<WifiSTA>  wifi = SystemInitializer::create_wifi(SSID, PASSWORD);
    std::unique_ptr<SNTPClient> sntp = SystemInitializer::create_sntp();

    while (true) {
        if (!SystemInitializer::connect_wifi(*wifi) ||
            !SystemInitializer::sync_time(*sntp, *wifi)) 
        {
            sleep(SystemInitializer::DEFAULT_RETRY_DELAY_SEC);
            continue;
        }

        printf("System ready! Starting alarm controller...\n");

        LedManager led_manager{IR_TRANSFER_PIN};
        AlertsManager alerts_manager{std::string(CITY)};
        AlarmController controller{alerts_manager, led_manager};

        try {
            controller.start();
        } catch (EspException& ex) {
            printf("[Main] ESP error %d: %s\n", ex.get(), esp_err_to_name(ex.get()));
        } catch (IRException& ex) {
            printf("[Main] IR error code: %d\n", static_cast<uint16_t>(ex.get()));
        } catch (Exception& ex) {
            printf("[Main] General error code: %d\n", static_cast<uint16_t>(ex.get()));
        } catch (...) {
            printf("[Main] Unknown error\n");
        }
    }
}
