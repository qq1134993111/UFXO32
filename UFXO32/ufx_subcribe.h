#pragma once
#include "ufx_connecter.h"
#include "ufx_resources.h"
#include<list>
#include<string>


class UfxSubcribe :public  CSubCallbackInterface, public CCallbackInterface, public UfxConnecter
{
public:
	UfxSubcribe(const UfxConfig::UfxConfigData& data);
	~UfxSubcribe();

	unsigned long  FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv) { return 0; }
	unsigned long  FUNCTION_CALL_MODE AddRef() { return 0; }
	unsigned long  FUNCTION_CALL_MODE Release() { return 0; }

	/**
	* 收到发布消息的回调
	* @param lpSub 回调的订阅指针
	* @param subscribeIndex 消息对应的订阅标识，这个标识来自于SubscribeTopic函数的返回
	* @param lpData 返回消息的二进制指针，一般是消息的业务体打包内容
	* @param nLength 二进制数据的长度
	* @param lpRecvData 主推消息的其他字段返回，主要包含了附加数据，过滤信息，主题名字，详细参看前面结构体定义
	* @return 无
	*/
	void FUNCTION_CALL_MODE OnReceived(CSubscribeInterface *lpSub, int subscribeIndex, const void *lpData, int nLength, LPSUBSCRIBE_RECVDATA lpRecvData);
	/**
	* 收到剔除订阅项的消息回调，一般在拥有踢人策略的主题下会回调这个接口,这个回调里面不需要取消订阅，底层已经取消这个订阅，只是一个通知接口
	* @param lpSub 回调的订阅指针
	* @param subscribeIndex 消息对应的订阅标识，这个标识来自于SubscribeTopic函数的返回
	* @param TickMsgInfo 踢人的错误信息，主要是包含具体重复的订阅项位置信息
	* @return 无
	*/
	void FUNCTION_CALL_MODE OnRecvTickMsg(CSubscribeInterface *lpSub, int subscribeIndex, const char* TickMsgInfo);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	* 套接字连接成功
	* @param lpConnection 发生该事件的连接对象
	*/
	virtual void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection)
	{
		std::cout << "Subcribe  OnConnect\n";
	}

	/**
	* 完成安全连接
	* @param lpConnection 发生该事件的连接对象
	*/
	virtual void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection)
	{

	}

	/**
	* 完成注册
	* @param lpConnection 发生该事件的连接对象
	*/
	virtual void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection)
	{

	}

	/**
	* 连接被断开
	* @param lpConnection 发生该事件的连接对象
	*/
	void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection)
	{
		std::cout << "Subcribe  OnClose\n";
		GetUfxProcessor().Post(&UfxSubcribe::ResetSubcribe, this);
	}

	/**
	* 发送完成
	* @param lpConnection 发生该事件的连接对象
	* @param hSend        发送句柄
	* @param reserved1    保留字段
	* @param reserved2    保留字段
	* @param nQueuingData 发送队列中剩余个数，使用者可以用此数值控制发送的速度，即小于某值时进行发送
	* @see Send()
	*/
	virtual void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
	{

	}

	virtual void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d)
	{

	}

	virtual void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d)
	{}

	virtual int  FUNCTION_CALL_MODE Reserved3()
	{
		return 0;
	}

	virtual void FUNCTION_CALL_MODE Reserved4()
	{}

	virtual void FUNCTION_CALL_MODE Reserved5()
	{}

	virtual void FUNCTION_CALL_MODE Reserved6()
	{}

	virtual void FUNCTION_CALL_MODE Reserved7()
	{}

	/**
	* 收到SendBiz异步发送的请求的应答
	* @param lpConnection    发生该事件的连接对象
	* @param hSend           发送句柄
	* @param lpUnPackerOrStr 指向解包器指针或者错误信息
	* @param nResult         收包结果
	* 如果nResult等于0，表示业务数据接收成功，并且业务操作成功，lpUnPackerOrStr指向一个解包器，此时应首先将该指针转换为IF2UnPacker *。
	* 如果nResult等于1，表示业务数据接收成功，但业务操作失败了，lpUnPackerOrStr指向一个解包器，此时应首先将该指针转换为IF2UnPacker *。
	* 如果nResult等于2，表示收到非业务错误信息，lpUnPackerOrStr指向一个可读的字符串错误信息。
	* 如果nResult等于3，表示业务包解包失败。lpUnPackerOrStr指向NULL。
	*/
	virtual void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult)
	{
		return;
	}

	/**
	* 收到SendBiz异步发送的请求的应答
	* @param lpConnection    发生该事件的连接对象
	* @param hSend           发送句柄
	* @param lpRetData 其他需要返回的应答内容，根据需要获取
	* @param lpUnPackerOrStr 指向解包器指针或者错误信息
	* @param nResult         收包结果
	* 如果nResult等于0，表示业务数据接收成功，并且业务操作成功，lpUnpackerOrStr指向一个解包器，此时应首先将该指针转换为IF2UnPacker *。
	* 如果nResult等于1，表示业务数据接收成功，但业务操作失败了，lpUnpackerOrStr指向一个解包器，此时应首先将该指针转换为IF2UnPacker *。
	* 如果nResult等于2，表示收到非业务错误信息，lpUnpackerOrStr指向一个可读的字符串错误信息。
	* 如果nResult等于3，表示业务包解包失败。lpUnpackerOrStr指向NULL。
	*/
	virtual void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
	{

	}
	//20130624 xuxp 回调增加BizMessage接口
	/**
	* 收到发送时指定了ReplyCallback选项的请求的应答或者是没有对应请求的数据
	* @param lpConnection 发生该事件的连接对象
	* @param hSend        发送句柄
	* @param lpMsg        业务消息指针
	*/
	virtual void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg) {}


	ErrorCode Subcribe();

	void DumpSubscribe(int sub_index, LPSUBSCRIBE_RECVDATA p_recv_data, std::basic_ostream<char>& ostream);

	std::string SubscribeToString(int sub_index, LPSUBSCRIBE_RECVDATA p_recv_data);

	bool IsSubcribe()
	{
		std::unique_lock<decltype(mtx_)> lc(mtx_);

		return (sp_sub_conn_ != nullptr&&sp_sub_interface_ != nullptr);
	}
	void ResetSubcribe()
	{
		std::unique_lock<decltype(mtx_)> lc(mtx_);

		if (sp_sub_interface_)
		{
			for (auto& pair_param : all_subscribe_param_map_)
			{
				auto i_ret = sp_sub_interface_->CancelSubscribeTopic(pair_param.first);
				std::cout << "CancelSubscribeTopic:" << i_ret << "," << (sp_conn_ != nullptr ? sp_conn_->GetErrorMsg(i_ret) : "");
			}
			sp_sub_interface_.reset();

		}

		if (sp_conn_)
		{
			sp_conn_.reset();
		}

		all_subscribe_param_map_.clear();
	}

	virtual void SubscribeHandle(std::shared_ptr<UfxUnpackerData> unpacker_data)
	{
		if (unpacker_data == nullptr || unpacker_data->GetUnPacker() == nullptr)return;

		std::string str = UnPackerToString(unpacker_data->GetUnPacker());
		std::cout << str;
		const char* p_msgtype = unpacker_data->GetUnPacker()->GetStr("msgtype");
		std::cout << "SubscribeHandle:msgtype[" << (p_msgtype ? p_msgtype : "") << "]\n";

	}

protected:
	std::shared_ptr<CConnectionInterface> sp_sub_conn_;
	std::shared_ptr<CSubscribeInterface> sp_sub_interface_;
	std::unordered_map<int, std::shared_ptr<CSubscribeParamInterface>> all_subscribe_param_map_;

};

