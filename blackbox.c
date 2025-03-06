
#include<xc.h>
#include<string.h>

#define _XTAL_FREQ 20000000

//i2c

void init_i2c() {
    TRISC3 = 1;
    TRISC4 = 1;

    SSPADD = 0x31;
    SMP = 1;
    CKE = 0;

    SSPEN = 1;
    SSPM3 = 1;
    SSPM2 = 0;
    SSPM1 = 0;
    SSPM0 = 0;

    SSPIF = 0;
    BCLIF = 0;
}

void i2c_start() {
    SEN = 1;
    while (!SSPIF);
    SSPIF = 0;
}

void i2c_write(unsigned char data) {
    SSPBUF = data;
    while (!SSPIF);
    SSPIF = 0;
}

void i2c_snt_ack() {
    ACKDT = 1;
    ACKEN = 1;
}

void i2c_re_start() {
    RSEN = 1;
    while (!SSPIF);
    SSPIF = 0;
}

void i2c_stop() {
    PEN = 1;
    while (!SSPIF);
    SSPIF = 0;
}

unsigned char i2c_read() {
    RCEN = 1;
    while (!SSPIF);
    SSPIF = 0;
    i2c_snt_ack();
    return SSPBUF;
}

unsigned char read_data(unsigned char d_addr, unsigned char r_addr) {
    i2c_start();
    i2c_write(d_addr);
    i2c_write(r_addr);
    i2c_re_start();
    i2c_write(d_addr + 1);
    unsigned char ch = i2c_read();
    i2c_stop();
    return ch;
}

void write_data(unsigned char d_addr, unsigned char r_addr, unsigned char data) {
    i2c_start();
    i2c_write(d_addr);
    i2c_write(r_addr);
    i2c_write(data);
    i2c_stop();
}
//rtc

void init_rtc() {
    unsigned char dummy;

    //set clock 24 hr do it
    dummy = read_data(0xD0, 0x02);
    write_data(0xD0, 0x02, dummy & 0x00);


    dummy = read_data(0xD0, 0x00);

    write_data(0xD0, 0x00, dummy & 0x7F);
}




// clcd

void write_clcd(unsigned char byte, unsigned char control_bit) {
    RC1 = control_bit;
    PORTD = byte;

    RC2 = 1;
    RC2 = 0;

    RC0 = 1;
    TRISD7 = 1;
    RC1 = 0;
    do {
        RC2 = 1;
        RC2 = 0;
    } while (RD7);
    TRISD7 = 0;
    RC0 = 0;
}

void init_clcd() {
    TRISD = 0x00;
    TRISC = TRISC & 0xF8;

    RC0 = 0;

    __delay_ms(30);

    write_clcd(0x33, 0);
    __delay_us(4100);
    write_clcd(0x33, 0);
    __delay_us(100);
    write_clcd(0x33, 0);
    __delay_us(10);

    write_clcd(0x01, 0);
    __delay_us(500);
    write_clcd(0x38, 0);
    __delay_us(100);
    write_clcd(0x0C, 0);
    __delay_us(100);
    write_clcd(0x02, 0);
    __delay_us(100);
}

void putch(unsigned char data, unsigned char addr) {
    write_clcd(addr, 0);
    write_clcd(data, 1);
}

void print(unsigned char *data, unsigned char addr) {
    write_clcd(addr, 0);
    while (*data) {
        write_clcd(*data++, 1);
    }
}

// mkp

void init_config_mkp() {
    TRISB = (TRISB & 0x01) | 0x1E;
    INTCON2bits.RBPU = 0;
}

unsigned char get_mkp_val() {
    RB5 = 0;
    RB6 = 1;
    RB7 = 1;
    if (RB1 == 0)
        return 3;
    if (RB2 == 0)
        return 4;
    if (RB3 == 0)
        return 6;
    RB5 = 1;
    RB6 = 0;
    RB7 = 1;
    if (RB1 == 0)
        return 1;
    if (RB4 == 0)
        return 11;
    RB5 = 1;
    RB6 = 1;
    RB7 = 0;
    RB5 = 1;
    RB6 = 1;
    RB7 = 0;
    if (RB1 == 0)
        return 2;
    if (RB4 == 0)
        return 12;
    return 5;
}

unsigned char read_mkp() {
    static int once = 1;
    unsigned char val = get_mkp_val();
    if (val != 5 && once) {
        once = 0;
        return val;
    } else if (val == 5) {
        once = 1;
    }
    return 5;
}

//read _potentiometer

void init_adc() {
    CHS3 = 0;
    CHS2 = 1;
    CHS1 = 0;
    CHS0 = 0;
    ADON = 1;

    PCFG3 = 1;
    PCFG2 = 0;
    PCFG1 = 1;
    PCFG0 = 0;

    ADFM = 1;
    ACQT2 = 0;
    ACQT1 = 0;
    ACQT0 = 1;

    ADCS2 = 0;
    ADCS1 = 1;
    ADCS0 = 0;

}

unsigned int read_adc() {
    unsigned int res;
    GO = 1;
    while (GO);
    res = ADRESH << 8 | ADRESL;
    return res;
}

void write_eeprom(unsigned char data, unsigned char addr) {
    
    write_data(0xA0, addr,data);
    for(int i=0;i<3000;i++);
//    EEDATA = data;
//    EEADR = addr;
//
//    EEPGD = 0;
//    CFGS = 0;
//
//    WREN = 1;
//    GIE = 0;
//    EECON2 = 0x55;
//    EECON2 = 0xAA;
//    WR = 1;
//    while (WR);
//    WREN = 0;
//    GIE = 1;
}

unsigned char read_eeprom(unsigned char addr) {
   unsigned char ch = read_data(0xA0,addr);
   return ch;
//    EEADR = addr;
//    EEPGD = 0;
//    CFGS = 0;
//    RD = 1;
//    return EEDATA;
}

void init_timer() {
    T08BIT = 1;
    T0CS = 0;
    PSA = 1;
    TMR0ON = 1;

    GIE = 1;
    TMR0IE = 1;
    TMR0IF = 0;
}
int i_count; //interrupt count

void __interrupt() isr() {
    if (TMR0IF == 1) {
        TMR0IF = 0;
        i_count++;
        if (i_count == 20000)
            i_count = 0;
    }
}

void init_config_uart()
{
    TRISC7 = 1;
    TRISC6 = 0;
    
    SPBRG = 129;

    
    TX9 = 0;
    TXEN = 1;
    SYNC = 0;
    SENDB = 0;
    BRGH = 1;
    
    SPEN = 1;
    RX9 = 0;
    CREN = 1;
    
    BRG16 = 0;
    
    TXIF = 0;
    RCIF = 0;
}

void write_char(unsigned char data)
{
    while(!TXIF);
    TXREG = data;
}

void write_string(unsigned char *data)
{
    while(*data)
    {
        write_char(*data++);
    }
}
// i have seen putting RCIF & TXIF to zero actually it is not need as per datasheet
unsigned char read_char()
{
    while(!RCIF);
    return RCREG;       
}

void read_string(unsigned char *buffer,int size)
{
    int i = 0;
    for(i=0;i<size-1;i++)
    {
        buffer[i] = read_char();
        if(buffer[i]=='\r')
            break;
    }
    buffer[i] = '\0';
}

unsigned char time[] = {"00:00:00"};
unsigned char event[9][3] = {"ON", "GN", "G1", "G2", "G3", "G4", "G5", "GR", "C_"};
unsigned int speed = 0;
unsigned long long int queue_index; //helps to store logs into eeprom 0 to 120bits 
int queue_count; //check how many logs stored in eeprom
int queue_count_flag; //to check if queue count reached 10 ,if 10 maintain the n.o items to be printed as 10 

void store_event(unsigned char *time, unsigned char*event, int speed) {

    while (*time) {
        write_eeprom(*time++, 0x01 + (queue_index % 120));
        queue_index++;
    }
    while (*event) {
        write_eeprom(*event++, 0x01 + (queue_index % 120));
        queue_index++;
    }
    write_eeprom(speed / 10 + 48, 0x01 + (queue_index % 120));
    queue_index++;
    write_eeprom((speed % 10) + 48, 0x01 + (queue_index % 120));
    queue_index++;
   write_eeprom(queue_count + 48, 0x00); //check for its usage;
}

unsigned char read_events(unsigned char(*arr)[17]) {

    int temp ;
    if(queue_count_flag==1)
        temp = 9;
    else
        temp = queue_count-1;//changed -2 to -1 ,,any further doubt print queue_count
    unsigned char addr = 0x00; //@0x00 im storing queue_count value
    int j = 0;
    int row = 0;
    int key = 5;
    int t_temp = temp;

    while (t_temp>=0) {
        int k = 0;
        t_temp--;
        for (int i = 0; i < 17; i++) {
            if (i == 10 || i == 13 || i == 1)
                arr[row][i] = ' ';
            else if (i == 0)
                arr[row][i] = row  + 48;
            else {
                k++;
                arr[row][i] = read_eeprom(0x00 + ((j + k) % 120));
            }

        }
        arr[row][16] = '\0';
        j += 12; //after accessing first 12 bytes from next start to read from 12th location,nxt time start from 24th location..... 
        row++;
    }
    if(queue_count_flag==1)
    {
        unsigned char f_arr[10][17];
        unsigned char s_arr[10][17];
        for(int i=0;i<10;i++)
        {
            if(i<queue_count)
            {
                strcpy(f_arr[i],arr[i]);
            }
            else{
                strcpy(s_arr[i-queue_count],arr[i]);
            }
        }
        int z=0;
        for(int i=0;i<10;i++)
        {
            if(i<10-queue_count)
            {
                strcpy(arr[i],s_arr[i]); 
            }
            else{
                strcpy(arr[i],f_arr[z++]);
            }
        }
        
    }
    for(int i=0;i<10;i++)
        {
            arr[i][0] = i+48;
        }
    if(temp>=0){
    print("IN  TIME   EV  SP", 0x80);
    print(&arr[0][0], 0xC0);}

    int i = 0;

    while (1) {//check its the problem
        if (temp >= 0||queue_count_flag) {
            key = read_mkp();
            if (key == 1) {
                write_clcd(0x01, 0);
                __delay_us(500);
                write_clcd(0x02, 0);
                __delay_us(100);
                i++;
                if (i >= temp)
                    i = temp-1;
                if(i==-1)
                print("IN  TIME   EV  SP", 0x80);
                else
                print(&arr[i][0], 0x80);
                print(&arr[i + 1][0], 0xC0);
            }
            if (key == 3) {
                write_clcd(0x01, 0);
                __delay_us(500);
                write_clcd(0x02, 0);
                __delay_us(100);
                i--;
                if (i <0)
                    i = 0;
                if(i==0)
                {
                  print("IN  TIME   EV SP", 0x80);
    print(&arr[0][0], 0xC0);  
                }
                else{
                print(&arr[i][0], 0x80);
                print(&arr[i + 1][0], 0xC0);}
            }
        } else {
            print("   log table     ", 0x80);
            print("     empty      ", 0xC0);
            __delay_ms(3000);
            return 0;
        }

        if (key == 12)
            return 0;
    }
}


void clear() {
    queue_count = 0;
    print(" memory cleared ", 0x80);
    print("   successful   ", 0xC0);
    __delay_ms(2000);

}



void download_log(unsigned char (*arr)[17]) 
{
    read_char();
    int temp;
    if(queue_count_flag)
        temp = 10;
    else
        temp = queue_count;
    write_string("IN  TIME   EV  SP\n\r");
    for(int i=0;i<temp;i++)
    {
        write_string(arr[i]);
        write_string("\n\r");
        if(i==9)
            break;
            
    }
    print("  download log   ",0x80);
    print("  succesful      ",0xC0);
    __delay_ms(5000);
    return;
}

void copy_to_temp(unsigned char*temp, unsigned char *td_arr, int start) {
    temp[0] = td_arr[start];
    temp[1] = td_arr[start + 1];
    td_arr[start] = ' ';
    td_arr[start + 1] = ' ';
}

void copy_from_temp(unsigned char*temp, unsigned char *td_arr, int start) {
    td_arr[start] = temp[0];
    td_arr[start + 1] = temp[1];
}


void change_min_sec(unsigned char*temp) {
    int num = (temp[0] - 48)*10 + temp[1] - 48;
    num++;
    if (num >= 60)
        num = 0;
    temp[0] = num / 10 + 48;
    temp[1] = num % 10 + 48;
}

void change_hr(unsigned char*temp) {
    int num = (temp[0] - 48)*10 + temp[1] - 48;
    num++;
    if (num == 24)
        num = 0;
    temp[0] = num / 10 + 48;
    temp[1] = num % 10 + 48;
}

void set_time(unsigned char *td_arr) {
    int l_count = 6;
    unsigned char temp[3];
    temp[2] = '\0';
    copy_to_temp(temp, td_arr, l_count);
    unsigned char key;
    write_clcd(0x01, 0);
     __delay_us(500);
    while (1) {
        print("    set time    ",0x80);
        key = read_mkp();
        if (i_count < 10000)//timer
        {
            print(temp, 0xC0 + l_count);
        } else {
            print(td_arr, 0xC0);
        }
        if (key == 1) //change position
        {
            copy_from_temp(temp, td_arr, l_count);
            l_count -= 3;
            if (l_count < 0)
                l_count = 6;
            copy_to_temp(temp, td_arr, l_count);
        }
        if (key == 3)// change time
        {
            if (l_count == 6) {
                change_min_sec(temp);
            } else if (l_count == 3) {
                change_min_sec(temp);
            } else if (l_count == 0) {
                change_hr(temp);
            }

        }
        if (key == 11) {
            copy_from_temp(temp, td_arr, l_count);
            unsigned char hr = (td_arr[0]-'0')<<4 | (td_arr[1]-'0');
    unsigned char min = (td_arr[3] - '0')<<4 | (td_arr[4]-'0'); 
    unsigned char sec = (td_arr[6] -'0')<<4 | (td_arr[7]-'0');
    write_data(0xD0,0x00,sec);
    write_data(0xD0,0x01,min);
    write_data(0xD0,0x02,hr);

            return;
        }
        if(key == 12)
            return;

    }
}
int once = 1;
void print_lcd_main_menu() {

    unsigned char*menu[4]; //store main menu
    unsigned char arr[10][17]; //to store details from eeprom
    int count = 0; //variable that make scrolling happens
    char key; //read mkp_value
    //char entered = 0;
    int current = 0; //variable making selection happens
    menu[0] = "view log        ";
    menu[1] = "download log    ";
    menu[2] = "clear log       ";
    menu[3] = "set time        ";
   __delay_ms(500);
    for (int i = 0; i < 10; i++)
        arr[i][16] = '\0';
    //strcpy(arr[0], "IND TIME   EV SP");

    if (once) {
        print("->", 0x80);
        print(menu[0], 0x82);
        print(menu[1], 0xC0);
        once = 0;
    }

    while (1) {
        key = read_mkp();
        if (key == 1 && count != 3) {
            print(menu[count], 0x80);
            print("->", 0xC0);
            current = count + 1;
            print(menu[count + 1], 0xC2);
            count++;
            //entered = 0; i think no use check tomorrow
            // check by giving delay if switching happens multiple times 
            __delay_ms(500);
        }
        if (key == 3 && count != 0) {
            print("->", 0x80);
            count--;
            current = count;
            print(menu[count], 0x82);
            print(menu[count + 1], 0xC0);
            // check by giving delay if switching happens multiple times 
            __delay_ms(500);
        }

        if (key == 11) {
            if (current == 0) {
                read_events(arr);
                //entered = 1;
            } else if (current == 2) {
                clear();
                //entered = 1;
            } else if (current == 1){
                print("  download log   ",0x80);
                print("  in progress      ",0xC0);
                download_log(arr);
            }
            else if (current == 3)
                set_time(time);
            print("->", 0x80);
            print(menu[0], 0x82);
            print(menu[1], 0xC0);
            count = 0;
            current = 0;

        }
        if (key == 12)
        {
            once=1;
            write_clcd(0x01, 0);
             __delay_us(500);
            return;
        }
    }
}

void get_time() {
    unsigned char temp[3];
    temp[0] = read_data(0xD0, 0x02); //hr
    temp[1] = read_data(0xD0, 0x01); //min
    temp[2] = read_data(0xD0, 0x00); //sec

    time[0] = 48 + ((temp[0] >> 4) & 0x03);
    time[1] = 48 + (temp[0] & 0x0F);
    time[2] = ':';
    time[3] = 48 + ((temp[1] >> 4) & 0x07);
    time[4] = 48 + (temp[1]&0x0F);
    time[5] = ':';
    time[6] = 48 + ((temp[2] >> 4)& 0x07);
    time[7] = 48 + (temp[2]&0x0F);
    time[8] = '\0';
}

void dashbox() {

    static int index = 0, flag = 0;
    static unsigned char key = 5;

    print(&event[index][0], 0xC0 + 10);
    print("Time      Ev  Sp", 0x80);
    print(time, 0xC0);
    speed = read_adc() / 10.33;
    putch((speed / 10) + 48, 0xC0 + 14);
    putch((speed % 10) + 48, 0xC0 + 15);

    key = read_mkp();
     if (key == 11) {

            print_lcd_main_menu();
        }
    if (key == 1) {
        index++;
        if (index == 8)
            index = 7;
        if (flag == 1) {
            index = 1;
            flag = 0;
        }
        store_event(time, &event[index][0], speed);
        queue_count++;
        print(&event[index][0], 0xC0 + 10);
    }
    if (key == 2) {
        index--;
        if (index == 0)
            index = 1;
        if (flag == 1) {
            index = 1;
            flag = 0;
        }
        store_event(time, &event[index][0], speed);
        queue_count++;
        print(&event[index][0], 0xC0 + 10);
    }
    if (key == 3) {
        index = 8;
        store_event(time, &event[index][0], speed);
        queue_count++;
        print(&event[index][0], 0xC0 + 10);
        flag = 1; //to indicate collision has occurred so start from GN ie index = 1;
    }
    if (queue_count > 9) {
        queue_count_flag = 1;

    }
    if (queue_count_flag == 1)
        queue_count = queue_count%10;

}



void main() {
    init_clcd();
    init_config_mkp();
    init_adc();
    init_i2c();
    init_rtc();
    init_config_uart();
    init_timer();
    // init_config_rtc();
    //char ch = read_eeprom(0x00);
    int wait = 0;
    int m_key = 5;

    queue_count = 0;
    //get_time();
    while (1) {


        
        m_key = read_mkp();
        if (wait++ == 1) {
            wait = 0;
            get_time();
            dashbox();
        }
        if (m_key == 11) {

            print_lcd_main_menu();
        }


    }
}
