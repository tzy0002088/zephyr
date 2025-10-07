#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include "uart.h"
#include "zephyr/irq.h"

#define DT_DRV_COMPAT inspire_isp_uart

static int uart_inspire_init(const struct device *dev)
{
    uart_init();
    return 0;
}

static void uart_inspire_poll_out(const struct device *dev, unsigned char c)
{
    uart_putc(c);
}

static int uart_inspire_poll_in(const struct device *dev, unsigned char *c)
{
    if (UART1->USR2 & 0x1)
    {
        *c = UART1->URXD & 0xff;
        return 0;
    }
    return -1;
}

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
static int uart_inspire_fifo_fill(const struct device *dev, const uint8_t *tx_data,int len)
{
    return 0;
}

static int uart_inspire_fifo_read(const struct device *dev, uint8_t *rx_data,const int size)
{
    return 0;
}

static void uart_inspire_irq_tx_enable(const struct device *dev)
{

}

static void uart_inspire_irq_tx_disable(const struct device *dev)
{

}

static int uart_inspire_irq_tx_ready(const struct device *dev)
{
    return 0;

}

static void uart_inspire_irq_rx_enable(const struct device *dev)
{

}

static void uart_inspire_irq_rx_disable(const struct device *dev)
{
}

static int uart_inspire_irq_tx_complete(const struct device *dev)
{
    return 0;
}

static int uart_inspire_irq_rx_ready(const struct device *dev)
{
    return 0;
}

static void uart_inspire_irq_err_enable(const struct device *dev)
{

}

static void uart_inspire_irq_err_disable(const struct device *dev)
{
}

static int uart_inspire_irq_is_pending(const struct device *dev)
{
    return 0;
}

static int uart_inspire_irq_update(const struct device *dev)
{
	return 1;
}

void uart_inspire_irq_callback_set(const struct device *dev,uart_irq_callback_user_data_t cb,void *user_data)
{

}
#endif //CONFIG_UART_INTERRUPT_DRIVEN

static const struct uart_driver_api uart_inspire_driver_api = {
    .poll_in = uart_inspire_poll_in,
    .poll_out = uart_inspire_poll_out,

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .fifo_fill = uart_inspire_fifo_fill,
    .fifo_read = uart_inspire_fifo_read,
    .irq_tx_enable = uart_inspire_irq_tx_enable,
    .irq_tx_disable = uart_inspire_irq_tx_disable,
    .irq_tx_ready = uart_inspire_irq_tx_ready,
    .irq_rx_enable = uart_inspire_irq_rx_enable,
    .irq_rx_disable = uart_inspire_irq_rx_disable,
    .irq_tx_complete = uart_inspire_irq_tx_complete,
    .irq_rx_ready = uart_inspire_irq_rx_ready,
    .irq_err_enable = uart_inspire_irq_err_enable,
    .irq_err_disable = uart_inspire_irq_err_disable,
    .irq_is_pending = uart_inspire_irq_is_pending,
    .irq_update = uart_inspire_irq_update,
    .irq_callback_set = uart_inspire_irq_callback_set,
#endif  /* CONFIG_UART_INTERRUPT_DRIVEN */
};

#define UART_INSPIRE_INIT(n) \
    DEVICE_DT_INST_DEFINE(n, \
                         uart_inspire_init, \
                         NULL, \
                         NULL, \
                         NULL, \
                         PRE_KERNEL_1, \
                         30, \
                         &uart_inspire_driver_api)

DT_INST_FOREACH_STATUS_OKAY(UART_INSPIRE_INIT)