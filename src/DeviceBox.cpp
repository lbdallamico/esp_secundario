#include "settings_ht.h"

struct_msg DeviceBox::_local_data;
struct_msg DeviceBox::_recevid_data;
UpdateComunication DeviceBox::_is_data_coming;
uint8_t DeviceBox::_lost_box = 0;

static std::vector<String> name_routine = {
    "INITIAL",
    "CONTINUIDADE_CABO_1",
    "CONTINUIDADE_CABO_2",
    "CONTINUIDADE_CABO_3",
    "CONTINUIDADE_CABO_4",
};

static std::vector<String> name_event = {
    "STARTING",
    "ERROR_MODE",
    "PERIPHEL_OFF",
    "PERIPHEL_ON",
};

static std::vector<String> name_feedback = {
    "ANY_TEST_RUNNING",
    "TEST_1_PASS",
    "TEST_1_FAIL",
    "TEST_2_PASS",
    "TEST_2_FAIL",
    "TEST_3_PASS",
    "TEST_3_FAIL",
    "TEST_4_PASS",
    "TEST_4_FAIL",
};

static std::vector<String> name_box2 = {
    "DEAD",
    "ALIVE"
};

DeviceBox::DeviceBox(void){
    /*
        Construtor apenas para garantir espaço na memoria
    */
   ;
}

void DeviceBox::Initializer(uint8_t * broadcast_address)
{
    // clear trash values from struct
    // I cannot use memcpy because this variable are static!
    _local_data._box_alive = ALIVE;
    _local_data._event = STARTING;
    _local_data._routine = INITIAL;
    _recevid_data._box_alive = ALIVE; // temporaria gambiarra
    _recevid_data._event = STARTING;
    _recevid_data._routine = INITIAL;

    _lost_box = 0;

    memcpy(&__macAdress_receiver, broadcast_address, sizeof(__macAdress_receiver));
    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        _local_data._event = ERROR_MODE;
        return;
    }
    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(DeviceBox::OnDataSent);
    // Register peer
    memcpy(peerInfo.peer_addr, __macAdress_receiver, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        _local_data._event = ERROR_MODE;
        return;
    }
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(DeviceBox::OnDataRecv);
}

DeviceBox::~DeviceBox(){}

ROUTINE_TEST DeviceBox::getCurrentRoutine(void)
{
    return _local_data._routine;
}

EVENT_SYSTEM DeviceBox::getCurrentStatus(void)
{
    return _local_data._event;
}

void DeviceBox::setResultTest(EVENT_SYSTEM result) {_local_data._event=result;}

void DeviceBox::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if (status == ESP_OK)
    {
        _lost_box = 0;
    }
    else
    {
        _lost_box++;
        _is_data_coming = ERRO;
    }
}

void DeviceBox::OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
    memcpy(&_recevid_data, incomingData, sizeof(_recevid_data));
    _is_data_coming = YES;
}

void DeviceBox::setFeedbacTest(FEEDBACK_TEST feedback) {_local_data._feedback_test = feedback;}

void DeviceBox::Send_Message(void)
{
    esp_err_t result = esp_now_send(__macAdress_receiver, 
                                    (uint8_t *) &_local_data, 
                                    sizeof(_local_data));
    if (result == ESP_OK) {
        //Serial.println("Sent with success");
    }
    else {
        Serial.println("Error sending the data");
    }
}

int DeviceBox::Process(void)
{
    /*
        Semrpre que houver qualquer alteração no sistema
        seja estado ou evento, os dois sistemas precisam
        ficar sabendo
    */
    if (_is_data_coming == YES)
    {
        _local_data._routine = _recevid_data._routine;
        _local_data._event = _recevid_data._event;
        if (_local_data._box_alive != _recevid_data._box_alive)
        {
            _local_data._box_alive = ALIVE;
        }
        Send_Message();
        return 1;
    }
    return 0;
}

void DeviceBox::Debug_SeeVariables()
{
    Serial.println("------------------------------");
    Serial.print("EVENT   : ");
    Serial.println(name_event[_local_data._event]);
    Serial.print("ROUTINE : ");
    Serial.println(name_routine[_local_data._routine]);
    Serial.print("BOX 2   : ");
    Serial.println(name_box2[_local_data._box_alive]);
    Serial.print("FEEDBACK: ");
    Serial.println(name_feedback[_local_data._feedback_test]);
}