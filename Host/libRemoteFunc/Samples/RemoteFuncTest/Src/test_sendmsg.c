#include "wb_syslib_addon.h"
#include "hic_host.h"
#include "FTH_Int.h"
#include "RemoteFunc.h"
#include "RemoteNet.h"
#include "clk.h"
#include "pll.h"
#include "tt_thread.h"

#include "RemoteFuncTest.h"
#include "test_sendmsg.h"

void test_sendmsg(void)
{
	test_printf_begin("test_sendmsg");
	test_sendmsg_entry();
	test_printf_end("test_sendmsg");
}

static void test_tcp_socket_thread_join(int ithread_handle)
{
	cyg_thread_info	info;
	cyg_uint16		id = 0;
	cyg_bool		rc;
	
	cyg_handle_t	next_thread = 0;
	cyg_uint16		next_id = 0;
	
	while(cyg_thread_get_next(&next_thread, &next_id) != false)
	{
		if(ithread_handle == next_thread)
		{
			id = next_id;
			break;
		}
	}
	
	while(1)
	{
		rc = cyg_thread_get_info(ithread_handle, id, &info);
		if(rc == false)
		{
			printf("Error get thread %d info\n", ithread_handle);
			break;
		}
		else
		{
			if((info.state & EXITED) != 0)
			{
				break;
			}
		}
		cyg_thread_yield();
	}
}

static TEST_SENDMSG_DATA_T netdata[TEST_SENDMSG_SERVER_NUM];

void test_sendmsg_entry(void)
{
	int iPort = TEST_SENDMSG_SERVER_BEGIN_PORT;
	int i;
	
	for(i = 0; i < TEST_SENDMSG_SERVER_NUM; i++)
	{
		netdata[i].iport = iPort+i;
		netdata[i].pbuf = (char*)NON_CACHE(g_RemoteNet_Buf1[i]);
		cyg_thread_create(THREAD_PRIORITY, &test_sendmsg_server, (cyg_addrword_t)&netdata[i], 
						NULL, thread_stack[i], STACK_SIZE, &thread_handle[i], &thread[i]);
		cyg_thread_resume(thread_handle[i]);
	}
	
	for(i = 0; i < TEST_SENDMSG_SERVER_NUM; i++)
	{
		test_tcp_socket_thread_join(thread_handle[i]);
		cyg_thread_delete(thread_handle[i]);
	}
}

void test_sendmsg_server(cyg_addrword_t pnetdata)
{
	int s, i, sendmsglen;
	struct sockaddr_in sa, r_sa;
	struct hostent *hp;
	struct msghdr msghdr_msg;
	struct iovec *piov;
	int j;
	int perIov_len;
	char tmp[TEST_SENDMSG_MSG_LEN];
	
	int threadid;
		
	int port = ((TEST_SENDMSG_DATA_T*)pnetdata)->iport;
	char *pbuf = ((TEST_SENDMSG_DATA_T*)pnetdata)->pbuf;
	
	threadid = port;
	
    if(inet_aton(TEST_SENDMSG_SERVER_ADDR, &sa.sin_addr, pbuf, RNT_BUFFER_LEN) == 0)
    {
		test_printf_error("test_sendmsg_server");
		cyg_thread_exit();
    }
    sa.sin_family = AF_INET;
    sa.sin_port = htons(IPPORT_USERRESERVED + port);
    
	if((hp = gethostbyname(TEST_REMOTEFUNC_HOSTNAME, pbuf, RNT_BUFFER_LEN)) == NULL)
	{
		test_printf_error("test_sendmsg_server");
		cyg_thread_exit();
	}
	
	memcpy(&(r_sa.sin_addr), hp->h_addr_list0, hp->h_length);
	r_sa.sin_family = AF_INET;
	r_sa.sin_port = htons(IPPORT_USERRESERVED + port);
	
	if((s = socket(AF_INET, SOCK_STREAM, PF_UNSPEC, pbuf, RNT_BUFFER_LEN)) == -1)
	{
		test_printf_error("test_sendmsg_server");
		cyg_thread_exit();
	}
	
	if(set_reuseaddr(s, pbuf, RNT_BUFFER_LEN) == -1)
	{
		test_printf_error("test_sendmsg_server");
		netclose(s, pbuf, RNT_BUFFER_LEN);
		cyg_thread_exit();
	}
	
	if(bind(s, (struct sockaddr*)&r_sa, sizeof(r_sa), pbuf, RNT_BUFFER_LEN) == -1)
	{
		test_printf_error("test_sendmsg_server");
		netclose(s, pbuf, RNT_BUFFER_LEN);
		cyg_thread_exit();
	}

    if(connect(s, (struct sockaddr*)&sa, sizeof(sa), pbuf, RNT_BUFFER_LEN) == -1)
    {
		test_printf_error("test_sendmsg_server");
		cyg_thread_exit();
    }
	
    for(i = 0; i < TEST_SENDMSG_WRITE_TIMES; i++)
	{
		strcpy(tmp, TEST_SENDMSG_MSG);
		
		perIov_len = TEST_SENDMSG_MSG_PART_LEN;
		piov = malloc(TEST_SENDMSG_MSG_PARTS*sizeof(struct iovec));
		if(piov == NULL)
		{
			test_printf_error("test_sendmsg_server");
			goto fail;
		}
		memset(piov, 0, TEST_SENDMSG_MSG_PARTS*sizeof(struct iovec));
		
		for(j = 0; j < TEST_SENDMSG_MSG_PARTS; j++)
		{
			piov[j].iov_base = malloc(perIov_len);
			if(piov[j].iov_base == NULL)
			{
				test_printf_error("test_sendmsg_server");
				for(i = 0; i < j; i++) free(piov[j].iov_base);
				if(piov != NULL) free(piov);
				goto fail;
			}
			piov[j].iov_len = perIov_len;
			memcpy(piov[j].iov_base, tmp + j * TEST_SENDMSG_MSG_PART_LEN, TEST_SENDMSG_MSG_PART_LEN);
		}
		
		msghdr_msg.msg_name = NULL;
		msghdr_msg.msg_namelen = 0;
		msghdr_msg.msg_iov = piov;
		msghdr_msg.msg_iovlen = TEST_SENDMSG_MSG_PARTS;
		msghdr_msg.msg_control = NULL;
		msghdr_msg.msg_controllen = 0;
		msghdr_msg.msg_flags = 0;
		
		sendmsglen = sendmsg(s, &msghdr_msg, 0, pbuf, RNT_BUFFER_LEN);
		if(sendmsglen < 0)
		{
			test_printf_error("test_sendmsg_server");
			for(j = 0; j < TEST_SENDMSG_MSG_PARTS; j++)
			{
				if(piov[j].iov_base != NULL) free(piov[j].iov_base);
			}
			if(piov != NULL) free(piov);
			break;
		}
		
		for(j = 0; j < TEST_SENDMSG_MSG_PARTS; j++)
		{
			if(piov[j].iov_base != NULL) free(piov[j].iov_base);
		}
		if(piov != NULL) free(piov);
	}
	
	test_printf_success("test_sendmsg_server");
	
fail:	
	netclose(s, pbuf, RNT_BUFFER_LEN);
	cyg_thread_exit();
}
