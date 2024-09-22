#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include "cproc.h"


#define DRIVER_NAME "spi_sysfs_dt_example"
#define SYSFS_ENTRY "cproc"

// Buffer to store received data
static struct kobject *root_kobj;
static custom_prot_frame_t input_dataframe = {0};
static custom_prot_frame_t output_dataframe = {0};
static uint8_t input_buffer[sizeof(custom_prot_frame_t)];
struct spi_device *spi_dev;

static uint16_t calculate_crc(const uint8_t *data, uint8_t length) {
    uint16_t crc = 0xFFFF; 
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001; 
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}
static int build_spi_frame(uint8_t msg_type, const uint8_t *payload, uint8_t payload_length, uint8_t *output_frame) {
    if (payload_length > MAX_PAYLOAD_SIZE) {
        return -1;
    }
    output_frame[0] = 0xAA;  
    output_frame[1] = msg_type;
    output_frame[2] = payload_length;
    if(payload) {
        memcpy(&output_frame[3], payload, payload_length);
    }
    uint16_t crc = calculate_crc(output_frame, 3 + payload_length);
    output_frame[3 + payload_length] = (crc >> 8) & 0xFF;
    output_frame[4 + payload_length] = crc & 0xFF;
    return 5 + payload_length;
}

int parse_spi_frame(const uint8_t *data, custom_prot_frame_t *frame) {
    frame->start_byte = data[0];
    frame->msg_type = data[1];
    frame->length = data[2];

    if (frame->length > MAX_PAYLOAD_SIZE) {
        return -2;
    }

    memcpy(frame->payload, &data[3], frame->length);

    frame->crc = (data[frame->length + 3] << 8) | data[frame->length + 4];
    printk("CRC: %d", frame->crc);
    uint16_t calculated_crc = calculate_crc(data, frame->length+3);
    if (calculated_crc != frame->crc) {
        return -3; 
    }

    if (frame->start_byte != 0xAA) {
        return -4;
    }

    return 0;
}


// Sysfs attribute to read SPI data
static ssize_t spi_data_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%s\n", "spi_data");
}
static ssize_t temperature_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    uint8_t buffer[1];
    int ret;
    buffer[0] = GET_TEMP; 
    int msg_length = build_spi_frame(CPROC_MSG_CMD, buffer, 1, (uint8_t*)&output_dataframe);
    struct spi_message msg;
    struct spi_message msgsecond;
    spi_message_init(&msg);
    spi_message_init(&msgsecond);
    struct spi_transfer transfer = {
        .tx_buf = &output_dataframe,
        .rx_buf = NULL,
        .len = (msg_length),
    };
    printk("Loading %d ", msg_length);
    spi_message_add_tail(&transfer, &msg);
    ret = spi_sync(spi_dev, &msg);
    mdelay(1000); // wait for processing
    memset(input_buffer, 0, 8);
    struct spi_transfer transfer_rec = {
        .tx_buf = NULL,
        .rx_buf = input_buffer,
        .len = sizeof(input_buffer),
    };
    spi_message_add_tail(&transfer_rec, &msgsecond);
    ret = spi_sync(spi_dev, &msgsecond);
    ret = parse_spi_frame(input_buffer, &input_dataframe);
    printk("result %d", ret);
    printk("size %d", sizeof(input_buffer));
    printk("%02x\n", (input_dataframe.start_byte));
    printk("%02x\n", (input_dataframe.msg_type));
    return sprintf(buf, input_dataframe.payload);
}
static ssize_t attidute_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    uint8_t buffer[1];
    int ret;
    buffer[0] = GET_ATTIDUTE; 
    int msg_length = build_spi_frame(CPROC_MSG_CMD, buffer, 1, (uint8_t*)&output_dataframe);
    struct spi_message msg;
    struct spi_message msgsecond;
    spi_message_init(&msg);
    spi_message_init(&msgsecond);
    struct spi_transfer transfer = {
        .tx_buf = &output_dataframe,
        .rx_buf = NULL,
        .len = (msg_length),
    };
    spi_message_add_tail(&transfer, &msg);
    ret = spi_sync(spi_dev, &msg);
    mdelay(1000); // wait for processing
    memset(input_buffer, 0, 8);
    struct spi_transfer transfer_rec = {
        .tx_buf = NULL,
        .rx_buf = input_buffer,
        .len = sizeof(input_buffer),
    };
    spi_message_add_tail(&transfer_rec, &msgsecond);
    ret = spi_sync(spi_dev, &msgsecond);
    ret = parse_spi_frame(input_buffer, &input_dataframe);
    printk("result %d", ret);
    printk("size %d", sizeof(input_buffer));
    printk("%02x\n", (input_dataframe.start_byte));
    printk("%02x\n", (input_dataframe.msg_type));
    return sprintf(buf, input_dataframe.payload);
}
static ssize_t humidity_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    uint8_t buffer[1];
    int ret;
    buffer[0] = GET_HUMIDITY; 
    int msg_length = build_spi_frame(CPROC_MSG_CMD, buffer, 1, (uint8_t*)&output_dataframe);
    struct spi_message msg;
    struct spi_message msgsecond;
    spi_message_init(&msg);
    spi_message_init(&msgsecond);
    struct spi_transfer transfer = {
        .tx_buf = &output_dataframe,
        .rx_buf = NULL,
        .len = (msg_length),
    };
    spi_message_add_tail(&transfer, &msg);
    ret = spi_sync(spi_dev, &msg);
    mdelay(1000); // wait for processing
    memset(input_buffer, 0, 8);
    struct spi_transfer transfer_rec = {
        .tx_buf = NULL,
        .rx_buf = input_buffer,
        .len = sizeof(input_buffer),
    };
    spi_message_add_tail(&transfer_rec, &msgsecond);
    ret = spi_sync(spi_dev, &msgsecond);
    ret = parse_spi_frame(input_buffer, &input_dataframe);
    printk("result %d", ret);
    printk("size %d", sizeof(input_buffer));
    printk("%02x\n", (input_dataframe.start_byte));
    printk("%02x\n", (input_dataframe.msg_type));
    return sprintf(buf, input_dataframe.payload);
}

static struct kobj_attribute spi_data_attr = __ATTR(spi_data, 0444, spi_data_show, NULL);
static struct kobj_attribute temperature_attr = __ATTR(temperature, 0444, temperature_show, NULL);
static struct kobj_attribute attidute_data_attr = __ATTR(attidute, 0444, attidute_show, NULL);
static struct kobj_attribute humidity_data_attr = __ATTR(humidity, 0444, humidity_show, NULL);




static int spi_dt_probe(struct spi_device *spi) {
    int ret;
    if(spi != NULL) {
        spi_dev = spi;
    } else {
        dev_err(&spi->dev, "Failed to probe cproc device\n");
        return -ENOMEM;
    }
    memset(&output_dataframe, 0, sizeof(output_dataframe));
    memset(&input_dataframe, 0, sizeof(output_dataframe));
    root_kobj = kobject_create_and_add(SYSFS_ENTRY, NULL);
    if (!root_kobj) {
        dev_err(&spi->dev, "Failed to create sysfs directory\n");
        return -ENOMEM;
    }

    ret = sysfs_create_file(root_kobj, &spi_data_attr.attr);
    if (ret) {
        kobject_put(root_kobj);
        dev_err(&spi->dev, "Failed to create sysfs file\n");
        return ret;
    }
    ret = sysfs_create_file(root_kobj, &temperature_attr.attr);
    if (ret) {
        kobject_put(root_kobj);
        dev_err(&spi->dev, "Failed to create sysfs file\n");
        return ret;
    }
    ret = sysfs_create_file(root_kobj, &attidute_data_attr.attr);
    if (ret) {
        kobject_put(root_kobj);
        dev_err(&spi->dev, "Failed to create sysfs file\n");
        return ret;
    }
    ret = sysfs_create_file(root_kobj, &humidity_data_attr.attr);
    if (ret) {
        kobject_put(root_kobj);
        dev_err(&spi->dev, "Failed to create sysfs file\n");
        return ret;
    }

    uint8_t buffer[1];
    buffer[0] = HELLO; 
    int msg_length = build_spi_frame(COMMAND, buffer, 1, (uint8_t*)&output_dataframe);
    struct spi_message msg, msgsecond;
    struct spi_transfer transfer = {
        .tx_buf = &output_dataframe,
        .rx_buf = NULL,
        .len = sizeof(output_dataframe),
    };
    spi_message_init(&msg);
    spi_message_add_tail(&transfer, &msg);
    ret = spi_sync(spi, &msg);
    mdelay(100); // wait for processing
    // ---- //
    spi_message_init(&msgsecond);
    struct spi_transfer transfer_rec = {
        .tx_buf = NULL,
        .rx_buf = input_buffer,
        .len = sizeof(input_buffer),
    };
    spi_message_add_tail(&transfer_rec, &msgsecond);
    ret = spi_sync(spi, &msgsecond);
    printk("START %d",input_buffer[0]);
    printk("START %d",input_buffer[1]);
    printk("START %d",input_buffer[2]);
    printk("RETURN D %d", ret);
    ret = parse_spi_frame(input_buffer, &input_dataframe);
    printk("RETURN K %d", ret);
    if((input_dataframe.msg_type != COMMAND) || (input_dataframe.payload[0] != HELLO)) {
       return -1; 
    }

    if (ret < 0) {
        dev_err(&spi->dev, "SPI transfer failed\n");
        return ret;
    }

    dev_info(&spi->dev, "SPI device probed, data: %s\n", "ds");
    return 0;
}

static void  spi_dt_remove(struct spi_device *spi) {
    sysfs_remove_file(root_kobj, &spi_data_attr.attr);
    sysfs_remove_file(root_kobj, &temperature_attr.attr);
    sysfs_remove_file(root_kobj, &attidute_data_attr.attr);
    sysfs_remove_file(root_kobj, &humidity_data_attr.attr);
    kobject_put(root_kobj);
    dev_info(&spi->dev, "SPI device removed\n");
}

// SPI device ID table for non-Device Tree platforms
static const struct spi_device_id spi_ids[] = {
    { "cproc", 0 },
    { }
};
MODULE_DEVICE_TABLE(spi, spi_ids);

static const struct of_device_id spi_dt_ids[] = {
    { .compatible = "custom,cproc" },
    { }
};
MODULE_DEVICE_TABLE(of, spi_dt_ids);

static struct spi_driver spi_dt_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = spi_dt_ids,
    },
    .id_table = spi_ids,              // For non-Device Tree platforms
    .probe = spi_dt_probe,
    .remove = spi_dt_remove,
};

module_spi_driver(spi_dt_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lukasz Przybylik");
MODULE_DESCRIPTION("SPI driver for communication with low-power coprocessor");
MODULE_VERSION("0.1");
