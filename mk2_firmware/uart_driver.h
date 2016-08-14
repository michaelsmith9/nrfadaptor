/*
 * UART Driver file.
 * Author: Michael Smith
 * Created 12/8/16
 */
#define UART_RATE 51

void usart_init(void);
void USART_SendByte(uint8_t u8Data);
void USART_SendString(char *str, uint8_t len);
void USART_SendByteString(uint8_t u8Data);
