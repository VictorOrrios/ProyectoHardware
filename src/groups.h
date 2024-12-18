/**
 * @defgroup HAL Hardware Abstraction Layer
 * @brief Hardware abstraction layer for microcontroller peripherals
 * @details Provides a hardware-independent interface for:
 *          - GPIO management
 *          - Timer operations
 *          - UART communication
 *          - External interrupts
 *          - Power management
 *          - Random number generation
 *          - Watchdog timer
 *          - Critical sections
 *
 * @defgroup HAL_LPC LPC2105 HAL Implementation
 * @ingroup HAL
 * @brief Hardware abstraction layer implementation for LPC2105
 * @details Platform-specific implementation including:
 *          - ARM7TDMI-S core support
 *          - VIC interrupt controller
 *          - On-chip peripheral access
 *          - Power management features
 *          - Timer configurations
 *
 * @defgroup HAL_NRF nRF52840 HAL Implementation
 * @ingroup HAL
 * @brief Hardware abstraction layer implementation for nRF52840
 * @details Platform-specific implementation including:
 *          - Cortex-M4F core support
 *          - NVIC interrupt controller
 *          - Advanced power modes
 *          - High-precision timers
 *
 * @defgroup DRV Device Drivers
 * @brief Hardware-independent device drivers
 * @details Mid-level drivers providing:
 *          - LED control
 *          - Button handling
 *          - Display management
 *          - Power monitoring
 *          - System timing
 *          - Watchdog services
 *          - UART communication
 *
 * @defgroup SVC Services
 * @brief High-level service modules
 * @details Application-level services including:
 *          - Logging system
 *          - Statistics collection
 *          - Alarm management
 *          - Game logic
 *          - System monitoring
 *          - Diagnostic tools
 *
 * @defgroup RT Real-Time Core
 * @brief Core real-time functionality and event management
 * @details Real-time system components including:
 *          - Event dispatching
 *          - Task scheduling
 *          - Message queuing
 *          - Timing services
 *          - Priority management
 *
 * @defgroup RT_EVENT Event Management
 * @ingroup RT
 * @brief Event types and management system
 *
 * @defgroup RT_FIFO FIFO Queue
 * @ingroup RT
 * @brief Real-time FIFO queue implementation
 *
 * @defgroup SVC_LOG Logging Service
 * @ingroup SVC
 * @brief Logging and debug output functionality
 *
 * @defgroup SVC_STATS Statistics Service
 * @ingroup SVC
 * @brief Performance monitoring and statistics collection
 *
 * @defgroup TESTING Test Suite
 * @ingroup SVC
 * @brief System component testing and validation
 *
 * @defgroup SVC_ALARM Alarm Service
 * @ingroup SVC
 * @brief Real-time alarm and timer management
 *
 * @defgroup APP Application Layer
 * @brief Application-level implementations and games
 * @details This group contains all application-level modules:
 *          - LED blinking demonstrations
 *          - Event-based control systems
 *          - Interactive LED patterns
 *          - Simon Says memory game
 *          - Performance monitoring
 *          - Power management examples
 */