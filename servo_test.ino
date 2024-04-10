#include <Servo.h>
#include <string.h>

#define BUF_LEN (9)
#define N_SERVO (2)

Servo servos[N_SERVO];

void setup(void) {
    Serial.begin(115200);

    servos[0].attach(6);
    servos[1].attach(5);
}

struct Command {
    Servo* servo;
    uint16_t angle;
};

bool is_number(char ch) { return !(ch < '0' || ch > '9'); }

struct Command parse_buffer(char buf[BUF_LEN]) {
    struct Command ret = {.servo = NULL, .angle = 0};
    size_t buf_idx = 0;

    if (buf[buf_idx++] != 'S') {
        return ret;
    }

    if (buf[buf_idx] < '0' || buf[buf_idx] >= '0' + N_SERVO) {
        return ret;
    }

    ret.servo = &servos[(buf[buf_idx++]) - '0'];

    if (buf[buf_idx++] != ':' || buf[buf_idx++] != ' ') {
        return ret;
    }

    if (!is_number(buf[buf_idx]) || !is_number(buf[buf_idx + 1]) ||
        !is_number(buf[buf_idx + 2])) {
        return ret;
    }

    ret.angle = (uint16_t)atoi(&buf[buf_idx]);
    return ret;
}

void execute_cmd(struct Command cmd) { cmd.servo->write(cmd.angle); }

void loop(void) {
    char rx_buffer[BUF_LEN] = {'\0'};

    Serial.write("Ciao Carlo");

    Command cmd;
    size_t rx_buf_index = 0;

    while (1) {
        while (Serial.available()) {
            char rx_ch = Serial.read();
            if (rx_ch == 'S') {
                rx_buf_index = 0;
            }
            rx_buffer[rx_buf_index++] = rx_ch;

            if (rx_buf_index == sizeof(rx_buffer) - 1) {
                Serial.println(rx_buffer);
                cmd = parse_buffer(rx_buffer);
                if (cmd.servo == NULL) {
                    continue;
                }
                Serial.println(cmd.angle);
                cmd.servo->write(cmd.angle);
                // execute_cmd(cmd);
                rx_buf_index = 0;
            }
        }
    }
}
