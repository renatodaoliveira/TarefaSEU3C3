# Arquivo de projeto CMake gerado para o Pico

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == NÃO EDITE AS LINHAS ABAIXO para a extensão VS Code do Raspberry Pi Pico funcionar ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Tipo de placa")

# Importa o Raspberry Pi Pico SDK
include(pico_sdk_import.cmake)

# Define o projeto
project(tarefau3c3 C CXX ASM)


#add_compile_definitions(configUSE_CORE_AFFINITY=0)

# Inicializa o pico-sdk
pico_sdk_init()


# Caminho para o FreeRTOS
set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)

# Importa o FreeRTOS
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# Adiciona o diretório com o código modular
add_subdirectory(src)
# Adicione quaisquer bibliotecas solicitadas pelo usuário
target_link_libraries(tarefau3c3 
        
        )

# Modifique as linhas abaixo para habilitar/desabilitar saída via UART/USB
pico_enable_stdio_usb(tarefau3c3 1)
pico_enable_stdio_uart(tarefau3c3 0)
