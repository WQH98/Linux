#include "bsp_i2c.h"

/* 初始化I2C */
void i2c_init(I2C_Type *base) {
    base->I2CR &= ~(1 << 7);            // 关闭I2C
    base->IFDR = 0x15;                  // 640分频，66000000 / 640 = 103.125KHz
    base->I2CR |= (1 << 7);             // 使能I2C
}


/* start信号产生以及从机地址的发送 */
unsigned char i2c_master_start(I2C_Type *base, unsigned char address, enum i2c_direction direction) {
    /* 检查I2C是否忙线 */
    if((base->I2SR & (1 << 5))) {
        return I2C_STATUS_BUSY;     // 忙线直接返回1
    }

    /* 设置为主机模式 */
    base->I2CR |= (1 << 5);         // 设置为主机
    base->I2CR |= (1 << 4);         // 设置为发送

    /* 产生start信号 */
    base->I2DR = ((unsigned int)address << 1) | ((direction == kI2C_Read)? 1 : 0);

    return I2C_STATUS_OK;
}


/* stop信号 */
unsigned char i2c_master_stop(I2C_Type *base) {
    unsigned short timeout = 0xFFFF;                // 超时时间

    /* 清除I2CR的bit5~3 */
    I2C1->I2CR &= ~(1 << 3);
    I2C1->I2CR &= ~(1 << 4);                        // 接收模式
    I2C1->I2CR &= ~(1 << 5);                        // 从模式

    /* 等待I2C忙结束 */
    while(I2C1->I2SR & (1 << 5)) {
        timeout--;
        if(timeout == 0) {
            // 超时跳出
            return I2C_STATUS_TIMEOUT;
        }
    }
    return I2C_STATUS_OK;
}


/* repeated start信号 */
unsigned char i2c_master_repeated_start(I2C_Type *base, unsigned char address, enum i2c_direction direction) {
    /* 检查I2C是否忙或在从机模式下 */
    if((base->I2SR & (1 << 5)) && ((base->I2CR & (1 << 5)) == 0)) {
        return I2C_STATUS_BUSY;
    }

    base->I2CR |= (1 << 4);             // 发送模式
    base->I2CR |= (1 << 2);             // 生成一个repeated信号
    base->I2DR = ((unsigned int)address << 1) | ((direction == kI2C_Read)? 1 : 0);

    return I2C_STATUS_OK;
}


/* 错误检查和清除函数 */
unsigned char i2c_check_and_clear_error(I2C_Type *base, unsigned int status) {
    /* 先检查是否为仲裁丢失错误 */
    if(status & (1 << 4)) {
        base->I2SR &= ~(1 << 4);            // 清除标志位
        base->I2CR &= ~(1 << 7);            // 关闭I2C
        base->I2CR |= (1 << 7);             // 打开I2C
        return I2C_STATUS_ARBITRATIONLOST;
    }
    /* 检测到NOACK信号 */
    else if(status & (1 << 0)) {
        return I2C_STATUS_NAK;
    }
    return I2C_STATUS_OK;
}


/* 发送数据 */
void i2c_master_write(I2C_Type *base, const unsigned char *buf, unsigned int size) {
    /* 等待传输完成 */
    while(!(base->I2SR & (1 << 7)));

    /* 清除中断标志位 */
    base->I2SR &= ~(1 << 1);

    /* 设置I2C为发送 */
    base->I2CR |= (1 << 4);

    /* 开始发送 */
    while(size--) {
        base->I2DR = *buf++;                    // 将要发送的数据写入I2DR数据寄存器
        
        while(!(base->I2SR & (1 << 1)));        // 等待传输完成
        base->I2SR &= ~(1 << 1);

        /* 检查ACK */
        if(i2c_check_and_clear_error(base, base->I2SR)) {
            break;
        }
    }

    base->I2SR &= ~(1 << 1);
    i2c_master_stop(base);
}


/* 读数据 */
void i2c_master_read(I2C_Type *base, unsigned char *buf, unsigned int size) {
    volatile uint8_t dummy = 0;                     // 假读
    dummy++;

    /* 等待传输完成 */
    while(!(base->I2SR & (1 << 7)));
    base->I2SR &= ~(1 << 1);

    base->I2CR &= ~(1 << 3);
    base->I2CR &= ~(1 << 4);

    /* 如果只接收一个数据 那么要发送一个NAK */
    if(size == 1) {
        base->I2CR |= (1 << 3);
    }

    dummy = base->I2DR; /* 假读 */

    /* 开始接收数据 */
    while(size--) {
        while(!(base->I2SR & (1 << 1)));            // 等待传输完成
        base->I2SR &= ~(1 << 1);

        /* 数据发送完成 */
        if(size == 0) {
            i2c_master_stop(base);
        }

        /* 倒数第二个数据 */
        if(size == 1) {
            base->I2CR |= (1 << 3);                 // 发送NACK
        }

        *buf++ = base->I2DR;
    }
}



/*
 * @description	: I2C数据传输，包括读和写
 * @param - base: 要使用的IIC
 * @param - xfer: 传输结构体
 * @return 		: 传输结果,0 成功，其他值 失败;
 */
unsigned char i2c_master_transfer(I2C_Type *base, struct i2c_transfer *xfer)
{
	unsigned char ret = 0;
	 enum i2c_direction direction = xfer->direction;	

	base->I2SR &= ~((1 << 1) | (1 << 4));			/* 清除标志位 */

	/* 等待传输完成 */
	while(!((base->I2SR >> 7) & 0X1)){}; 

	/* 如果是读的话，要先发送寄存器地址，所以要先将方向改为写 */
    if ((xfer->subaddressSize > 0) && (xfer->direction == kI2C_Read))
    {
        direction = kI2C_Write;
    }

	ret = i2c_master_start(base, xfer->slaveAddress, direction); /* 发送开始信号 */
    if(ret)
    {	
		return ret;
	}

	while(!(base->I2SR & (1 << 1))){};			/* 等待传输完成 */

    ret = i2c_check_and_clear_error(base, base->I2SR);	/* 检查是否出现传输错误 */
    if(ret)
    {
      	i2c_master_stop(base); 						/* 发送出错，发送停止信号 */
        return ret;
    }
	
    /* 发送寄存器地址 */
    if(xfer->subaddressSize)
    {
        do
        {
			base->I2SR &= ~(1 << 1);			/* 清除标志位 */
            xfer->subaddressSize--;				/* 地址长度减一 */
			
            base->I2DR = ((xfer->subaddress) >> (8 * xfer->subaddressSize)); // 向I2DR寄存器写入子地址
  
			while(!(base->I2SR & (1 << 1)));  	/* 等待传输完成 */

            /* 检查是否有错误发生 */
            ret = i2c_check_and_clear_error(base, base->I2SR);
            if(ret)
            {
             	i2c_master_stop(base); 				/* 发送停止信号 */
             	return ret;
            }  
        } while ((xfer->subaddressSize > 0) && (ret == I2C_STATUS_OK));

        if(xfer->direction == kI2C_Read) 		/* 读取数据 */
        {
            base->I2SR &= ~(1 << 1);			/* 清除中断挂起位 */
            i2c_master_repeated_start(base, xfer->slaveAddress, kI2C_Read); /* 发送重复开始信号和从机地址 */
    		while(!(base->I2SR & (1 << 1))){};/* 等待传输完成 */

            /* 检查是否有错误发生 */
			ret = i2c_check_and_clear_error(base, base->I2SR);
            if(ret)
            {
             	ret = I2C_STATUS_ADDRNAK;
                i2c_master_stop(base); 		/* 发送停止信号 */
                return ret;  
            }     
        }
    }	

    /* 发送数据 */
    if ((xfer->direction == kI2C_Write) && (xfer->dataSize > 0))
    {
    	i2c_master_write(base, xfer->data, xfer->dataSize);
	}

    /* 读取数据 */
    if ((xfer->direction == kI2C_Read) && (xfer->dataSize > 0))
    {
       	i2c_master_read(base, xfer->data, xfer->dataSize);
	}
	return 0;	
}

