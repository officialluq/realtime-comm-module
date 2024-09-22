// Maksymalny rozmiar payloadu
#define MAX_PAYLOAD_SIZE 255

#define CPROC_MSG_CMD 0x01
#define CPROC_MSG_RES 0x02
#define CPROC_MSG_REQ 0x03

typedef enum {
    HELLO = 1,
    GET_TEMP = 2,
    GET_HUMIDITY = 3,
    GET_ATTIDUTE = 4,
    LED_RED = 5,
    LED_GREEN = 6,
    LED_BLUE = 7
} custom_command_t;

typedef enum {
    COMMAND = 1,
    MSG = 2,
    RESPONSE = 3
} custom_msg_types_t;
//+------------+----------------+--------------+--------------+-----------+
//| Start Byte | Msg Type (1B)  | Length (1B)  | Payload (Xb) | CRC (16b) |
//+------------+----------------+--------------+--------------+-----------+

// Struktura ramki SPI
typedef struct {
    uint8_t start_byte;       // Start byte (np. 0xAA)
    uint8_t msg_type;         // Typ wiadomości (komenda/odpowiedź/błąd)
    uint8_t length;           // Długość payloadu
    uint8_t payload[MAX_PAYLOAD_SIZE]; // Zmienny payload
    uint16_t crc;             // CRC do weryfikacji integralności
} custom_prot_frame_t;