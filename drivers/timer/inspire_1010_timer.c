#include <stdint.h>
#include <stdbool.h>
#include <sys/_stdint.h>
#include <zephyr/init.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys_clock.h>
#include <zephyr/spinlock.h>
#include <zephyr/irq.h>
#include "MCIMX6Y2.h"

// 一个 tick 为多少 timer cnt
#define CYC_PER_TICK (uint32_t)(sys_clock_hw_cycles_per_sec() / CONFIG_SYS_CLOCK_TICKS_PER_SEC)

/* the unsigned long cast limits divisions to native CPU register width */
#define cycle_diff_t   unsigned long
#define CYCLE_DIFF_MAX (~(cycle_diff_t)0)
#define CYCLES_MAX_1 ((uint64_t)INT32_MAX * (uint64_t)CYC_PER_TICK)
#define CYCLES_MAX_2 ((uint64_t)CYCLE_DIFF_MAX)
#define CYCLES_MAX_3 MIN(CYCLES_MAX_1, CYCLES_MAX_2)
#define CYCLES_MAX_4 (CYCLES_MAX_3 / 2 + CYCLES_MAX_3 / 4)
#define CYCLES_MAX   (CYCLES_MAX_4 + LSB_GET(CYCLES_MAX_4))

static struct k_spinlock lock;
static uint64_t last_cnt; // 上一次 timer 中断的时候，到了多少个 cnt ( cnt 寄存器的值)
static uint64_t last_ticks; // 上一次 timer 中断的时候，tick 为多少
static uint32_t last_elapsed; // 上次中断到现在，经过了多少 tick

void sys_clock_set_timeout(int32_t ticks, bool idle)
{
    if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
        return;
    }

    k_spinlock_key_t key = k_spin_lock(&lock);
    uint64_t cyc;

    if (ticks == K_TICKS_FOREVER) {
        cyc = last_cnt + CYCLES_MAX;
    } else {
        cyc = (last_ticks + last_elapsed + ticks) * CYC_PER_TICK;
        if ((cyc - last_cnt) > CYCLES_MAX) {
            cyc = last_cnt + CYCLES_MAX;
        }
    }
    GPT1->OCR[0] = cyc;
    k_spin_unlock(&lock, key);
}

uint32_t sys_clock_elapsed(void)
{
    if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
        return 0;
    }

    k_spinlock_key_t key = k_spin_lock(&lock);

    uint32_t now = GPT1->CNT;
    uint32_t dcycles = now - last_cnt;
    uint32_t dticks = (cycle_diff_t)dcycles / CYC_PER_TICK;
    last_elapsed = dticks;

    k_spin_unlock(&lock, key);

    return dticks;
}

static void gpt_isr(void *param)
{
    k_spinlock_key_t key = k_spin_lock(&lock);

    uint32_t now = GPT1->CNT;
    uint32_t dcycles = now - last_cnt;
    uint32_t dticks = dcycles / CYC_PER_TICK;

    GPT1->SR |= BIT(0);

    last_cnt += dticks * CYC_PER_TICK;
    last_ticks += dticks;
    last_elapsed = 0;

    if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
        uint64_t next = last_cnt + CYC_PER_TICK;
        GPT1->OCR[0] = next;
    }

    k_spin_unlock(&lock, key);
    sys_clock_announce(dticks);
}

uint32_t sys_clock_cycle_get_32(void)
{
    return GPT1->CNT;
}

static int sys_clock_driver_init(void)
{
    GPT1->CR = 1 << 15;
    while((GPT1->CR >> 15) & 0x01);

    GPT1->CR = (1<<6);
    GPT1->CR |= (1<<9);

    GPT1->PR = 65;
    GPT1->OCR[0] = CYC_PER_TICK; // 延时一个 tick
    GPT1->IR |= 1 << 0;

    IRQ_CONNECT(GPT1_IRQn, 0, gpt_isr, NULL, 0);
    irq_enable(GPT1_IRQn);
    GPT1->CR |= 1<<0;

    return 0;
}

SYS_INIT(sys_clock_driver_init, PRE_KERNEL_2, CONFIG_SYSTEM_CLOCK_INIT_PRIORITY);

