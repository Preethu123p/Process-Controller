
#define MAX_DATA_LEN  128

typedef enum 
{
  MSG_UNAME_PASSWD = 1,
  MSG_AUTH_SUCCESS = 2,
  MSG_DELETE_PROCESS = 3,
  MSG_AUTH_FAIL = 4,
  MSG_LOGIN = 5,
  MSG_MAX = 6,
  MSG_ADD_PROCESS =7

} msg_type_e;

#define DATA_SIZE 1000

typedef struct 
{
  msg_type_e  msg_type; 
  char        msg_data[MAX_DATA_LEN];
  //char        msg_add_data[MAX_DATA_LEN];

} msg_t;

typedef struct 
{
  void        (*msg_action_function) (int, msg_t *);

} msg_action_t;



int add();
