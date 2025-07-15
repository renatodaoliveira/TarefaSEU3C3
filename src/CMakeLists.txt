# Cria o executável chamado "tarefau3c3" a partir do arquivo main.c
add_executable(tarefau3c3
    main.c
)

# Define o diretório de saída dos binários para build/ (em vez de build/src/)
set_target_properties(tarefau3c3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Adiciona o arquivo de gerenciamento de heap do FreeRTOS ao projeto
target_sources(tarefau3c3
    PRIVATE
    ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_4.c
)

# Inclui diretórios de cabeçalhos necessários para o projeto
target_include_directories(tarefau3c3 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}           # Diretório atual (src)
    ${CMAKE_CURRENT_LIST_DIR}/../config # Configurações do projeto
    ${CMAKE_CURRENT_LIST_DIR}/../tasks  # Tarefas do projeto
    ${CMAKE_CURRENT_LIST_DIR}/../drivers# Drivers do projeto
)

# Faz o link do executável com as bibliotecas necessárias
target_link_libraries(tarefau3c3 
    pico_stdlib       # Biblioteca padrão do Pico SDK
    FreeRTOS-Kernel   # Núcleo do FreeRTOS
    hardware_adc      # Biblioteca de ADC do Pico SDK
)

# Gera arquivos extras (como UF2, bin, etc) para tarefau3c3
pico_add_extra_outputs(tarefau3c3 )
