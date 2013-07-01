/**
 * \file      UDS_Protocol.cpp
 * \brief     Definition file for CUDS_Protocol class
 * \author    Sanchez A
 *
 *  Manager of the UDS_Protocol Porject
 */

#pragma once
#include "StdAfx.h"
#define USAGE_EXPORT
#include "UDS_Extern.h"
#include "UDSMainWnd.h"
#include "UDS_NegRespMng.h"	 
#include "UDSSettingsWnd.h"
#include "UDS_SettingsWnd.h"
#include "UDS_TimmingWnd.h"
#include "UDS_Protocol.h"
#include "include/struct_can.h"


//#include "PSDI_CAN/MsgContainer_CAN.h"

CUDSMainWnd* omMainWnd = NULL;
CUDS_NegRespMng* NegRespManager = NULL; 
CUDSSettingsWnd* omSettingsWnd = NULL; 
CUDS_Protocol* omManagerPtr = NULL;
bool fWaitLongResponse = FALSE; 
bool FDontShow =FALSE;		// if it's TRUE the message should not be shown
							// This variable is used to check if the response corresponds to my request

float Length_Received =0;	// It's the number of bytes received in a long response

int numberOfTaken;			// It contains the number of Data bytes that can be sent in the current message
int initialByte;
int aux;					// Ayuda a controlar el index de i y a diferenciar entre el caso de Extended y Normal addressing
unsigned char abByteArr[64];
int STMin;					// Corresponds to the STMin received from the flowControlMessage
int BSize;
int BSizE;					// Corresponds to the BSize received from the flowControlMessage
int SizeFC;
int P2_Time=250;
int P2_Time_Extended =2000;
bool FSending = FALSE;			// This variable is used to know if a message has been sent from the UDSMainWnd
CString Bytes_to_Show= ("\r\n   1-> ");
float TotalFrames;   // This variable is used to determinate how much continuos frames will be received
int respID;   ////correspond to the exact ID of the message received
int RespMsgID;	 //Corresponds to the value put in the settingWnd if it's extended
bool fMsgSize = FALSE;		// If TRUE the msg should have 8 bytes always
bool FCRespReq = FALSE;		// If TRUE the msg should have 8 bytes always


int Lear = 1; 
int LengthLastFrame;
CString CurrentService;
UINT_PTR m_nTimer;



BEGIN_MESSAGE_MAP(CUDS_Protocol, CWinApp)
END_MESSAGE_MAP()


/**
* CUDS_Protocol construction
*/
CUDS_Protocol::CUDS_Protocol()
{	
	omManagerPtr=this;
	SourceAddress =0;
	TargetAddress =0;
	fInterface = INTERFACE_NORMAL_11;
	MsgID = 0x700;
	numberOfBytes = 1;
}

// The one and only CUDS_Protocol object
CUDS_Protocol theApp;

/**
* CUDS_Protocol initialization
*/

static HINSTANCE ghLangInst=NULL;

BOOL CUDS_Protocol::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}

/**********************************************************************************************************
 Function Name  :   DIL_UDS_ShowWnd

 Description    :   It's called from the MainFrm to show the MainWnd
 Member of      :   CUDS_Protocol

 Author(s)      :   S�nchez A
 Date Created   :   01.03.2013
**********************************************************************************************************/
USAGEMODE HRESULT DIL_UDS_ShowWnd(HWND hParent,int TotalChannels)
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());	   //Comentado por los momentos
	INT_PTR nRet = -1; 
	CWnd objParent;
	objParent.Attach(hParent);

	if (omMainWnd == NULL)
	{
		omMainWnd = new CUDSMainWnd(omManagerPtr->SourceAddress, omManagerPtr->TargetAddress, omManagerPtr->fInterface,omManagerPtr->MsgID,&objParent);
		omMainWnd->TotalChannel = TotalChannels;
		omMainWnd->Create(IDM_UDS);
		NegRespManager = new CUDS_NegRespMng(omManagerPtr);		
	}

	omMainWnd->ShowWindow(SW_SHOW);

	objParent.Detach();
	return 0;
}
/**********************************************************************************************************
 Function Name  :   UpdateChannelUDS

 Description    :   It's called from the MainFrm to update the channel count in the MainWnd
 Member of      :   CUDS_Protocol

 Author(s)      :   S�nchez A
 Date Created   :   01.06.2013
**********************************************************************************************************/

USAGEMODE HRESULT UpdateChannelUDS(HWND hParent)
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());	   //Comentado por los momentos

	if (omMainWnd != NULL)
	{   
		omMainWnd->vUpdateChannelIDInfo();

	}	
	//objParent.Detach();
	return 0;
}

/**********************************************************************************************************
 Function Name  :   Show_ResponseData

 Input(s)       :	psMsg[] contains the bytes to be shown, Datalen is the position inside of psMsg[]
					that contains the last byte to showt, posFirstByte is the location inside of psMsg[] 
					where this function should start to read. 
 Output         :	 -
 Description    :   This function is called by EvaluateMessage to show in the Response Data section 
					the bytes received in the bus as response of a request sent from the UDS tool.
 Member of      :   CUDS_Protocol

 Author(s)      :   S�nchez A
 Date Created   :   19.06.2013	    
/**********************************************************************************************************/

void CUDS_Protocol::Show_ResponseData(unsigned char psMsg[], unsigned char Datalen , int posFirstByte )
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int loc = Datalen;
	char hex [9];
	if(omMainWnd!=NULL ){	  
		//CString TempByte_Cstring = "";
		//Bytes_to_Show= Bytes_to_Show+ ("\r\n       ");				//looks like the consecutive frames

		int intResp;
		char auxArray[16];
		for (int ByteCounter = posFirstByte+1; ByteCounter<loc ;ByteCounter++){		  // Solo para mostrar los valores deseados
			UCHAR TempByte = (psMsg[ByteCounter]) ;
			int Temp_char2Int =  TempByte;				
			intResp = sprintf (hex,"%02X ",Temp_char2Int);
			Bytes_to_Show= Bytes_to_Show+ hex;  
			Lear++;
			numberOfBytes++;

			if (Lear == NUM_BYTES_SHOWN_RESP+1){
				CString CstringNumberOfBytes;
				CstringNumberOfBytes.Format("%d", numberOfBytes);
				int i =4;
				while(i- CstringNumberOfBytes.GetLength()>0){ //for (int i = 4; i- CstringNumberOfBytes.GetLength()>0 ; i--){
					CstringNumberOfBytes = " "+CstringNumberOfBytes ; 					
				}
				Lear = 1; 
				//intResp = sprintf (hex,"%03d-> ",numberOfBytes);
				Bytes_to_Show= Bytes_to_Show+ ("\r\n") +CstringNumberOfBytes +" -> ";
			
			}
		}  
		omMainWnd->m_abDatas = Bytes_to_Show+ '\0';

		omMainWnd->UpdateData(false);
	}
}
/**********************************************************************************************************
 Function Name  :   EvaluateMessage

 Description    :   Evaluate any message received from the ECU -Filter the message received and call
				    initialize it the variables to show it 
 Member of      :   

 Author(s)      :   S�nchez A
 Date Created   :   19.06.2013
**********************************************************************************************************/

USAGEMODE HRESULT EvaluateMessage(unsigned char psMsg[],  unsigned char Datalen, UINT MessageID )
{	
	// Datalen is the DLC of the message received
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(omMainWnd!=NULL && FSending && respID==MessageID ){		// Evaluates the received message when: 1.MainWnd created - 2. Response to my Msg - 3.I've sent something
		
		TYPE_OF_FRAME TypeofFrame = omManagerPtr->getFrameType(psMsg[initialByte]);		// This is a general function because it uses the variable initialByte
		
		if (FWaitFlow == TRUE && TypeofFrame ==  FLOW_CONTROL ){				//Am I waiting for a flow control?
			
			switch (omManagerPtr->TypeofControl){	  // Para evaluar como deben estar los Textbox del SA y TA
					case 0:{

						BSizE = psMsg[initialByte+1];
						if(psMsg[initialByte+2]<=0x7F){					// Evaluate all the possible Values for STmin 
							STMin = psMsg[initialByte+2];  						
						}else if( 0xF1<=psMsg[initialByte+2]<=0xF9){
							STMin = (psMsg[initialByte+2]&0x0F)*0.1;
						}else {
							STMin = 127;								 //According to the ISO-TP
						} 
						omMainWnd->SendContinuosFrames(  omMainWnd->psTxCanMsgUds->m_psTxMsg->m_ucData,omMainWnd->psTxCanMsgUds,omMainWnd->fInterface );

						FWaitFlow = FALSE;								// Don't wait for the flowcontrol anymore
						   }
						   break;
					case 1:
						{
							omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);					// Wait,ReStart Timer -> do not able the SEND button yet
							omMainWnd->SetTimer(ID_TIMER_SEND_BUTTON,P2_Time , NULL);				
						}
						break;	
					default:{				    // If I've received any reserved message or  an Overflow message. 
						FWaitFlow = FALSE;		// para indicar que ya NO debo esperar el flowControl
							}				    // If it's -1 then it's not a flow control message 
							//do nothing	 
			}

		} else if(TypeofFrame ==  LONG_RESPONSE){		 //Is it a long response?? 

			int TempValue = strtol(CurrentService, NULL, 16);
			if( psMsg[initialByte+2] == TempValue+0x40){	// Evaluate it only if it corresponds to my request
				omManagerPtr->numberOfBytes =1;
				Length_Received = Length_Received + psMsg[initialByte+1];	     // The length of the receivedMessage
				omMainWnd->PrepareFlowControl();		
				omManagerPtr->Show_ResponseData(psMsg ,Datalen,initialByte+1); // Por ahora esta funci�n est� definida en el UDSProtocol pero puede pasar a estar definida en MainWnd
				TotalFrames = (Length_Received - 6+ aux )/ (7-aux);			// This variable is used to determinate how much continuos frames will be received
				LengthLastFrame = ((int)Length_Received - 6+ aux )%(7-aux);
				omMainWnd->m_omDiagService = NegRespManager->evaluateResp(psMsg ,initialByte+1);
				omMainWnd->m_omSendButton.EnableWindow(FALSE);
				m_nTimer = omMainWnd->SetTimer(ID_TIMER_SEND_BUTTON,P2_Time /*TIME_UNABLE_SEND_BUTTON*/, NULL);
			}	
	
		} else if(TypeofFrame== CONSECUTIVE_FRAME){						  //Is it a consecutive Frame?
			Counter_BSize--;
			TotalFrames--; 
			omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);
			omMainWnd->SetTimer(ID_TIMER_SEND_BUTTON,P2_Time , NULL);				//ReStart Timer 

			if(TotalFrames==0){						 //It enters here when the last frame has exactly 7/6 bytes of data
				FSending = FALSE;					 //Stops showing the bytes in the mainWnd
				omMainWnd->m_omSendButton.EnableWindow(TRUE);
				omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);

			}else if(TotalFrames<0){				 //It enters here when the last frame has less than 7/6 bytes of data
				Datalen = LengthLastFrame+1+aux;	
				FSending = FALSE;					 //Stops showing the bytes in the mainWnd
				omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);
				omMainWnd->m_omSendButton.EnableWindow(TRUE); 
			}
			omManagerPtr->Show_ResponseData(psMsg ,Datalen, initialByte); // now show the response 
			if(Counter_BSize ==0) omMainWnd->PrepareFlowControl();		  // If I've received BSize frames-> Send the flow control

		} else {							  // This case corresponds to a simple response 
			omManagerPtr->numberOfBytes =1;
			FSending = FALSE;				  //Show this message and then Stop showing the bytes
			omMainWnd->m_omDiagService = NegRespManager->evaluateResp(psMsg , initialByte);			// Show the description of the Response 
			if(!FDontShow) Length_Received, omManagerPtr->Show_ResponseData(psMsg ,psMsg[initialByte]+initialByte+1, initialByte);	// If it's a simple response the legth  will be in psMsg[initialByte]
			FDontShow = FALSE;				  // The nextFramme can be shown

		}
		omMainWnd->m_omBytes.vSetValue(Length_Received); 
	}
	return 0;
}
/**********************************************************************************************************
 Function Name  :   getFrameType

 Description    :   It returns the type of frame that was received
 Member of      :   CUDS_Protocol

 Author(s)      :   S�nchez A
 Date Created   :   19.06.2013
**********************************************************************************************************/

TYPE_OF_FRAME CUDS_Protocol::getFrameType(BYTE FirstByte)  {							  
	BYTE Mask_FB = FirstByte&0xF0;
	int TempByte = FirstByte+(0x30); 

	if (0x60 <= TempByte && TempByte< 0x62){		//it supports all cases of Flow Control
		TypeofControl = FirstByte& 0x0F;
		return FLOW_CONTROL;
	}else if(Mask_FB == 0x10){
		Length_Received = (FirstByte&0x0F)<<8;		//long response
		return  LONG_RESPONSE; 
	}else if(Mask_FB == 0x20){
		return CONSECUTIVE_FRAME;					//If it's a consecutive frame the Length_Received doesn't change.   
	}
	Length_Received = FirstByte; 
	return SIMPLE_RESPONSE;
}
	
/**********************************************************************************************************
 Function Name  :   DIL_UDS_ShowSettingWnd

 Description    :   It's called from the MainFrm to show the settings Wnd
 Member of      :   CUDS_Protocol

 Author(s)      :   S�nchez A
 Date Created   :   01.03.2013
**********************************************************************************************************/

USAGEMODE HRESULT DIL_UDS_ShowSettingWnd(HWND hParent)
{
	//HRESULT hResult = S_OK;
	//DWORD hResult
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	INT_PTR nRet = -1; 
	CWnd objParent;
	objParent.Attach(hParent);

	if (omSettingsWnd == NULL)
	{ 
		omSettingsWnd = new CUDSSettingsWnd(&objParent,omManagerPtr);
		omSettingsWnd->Create(IDD_SETTINGS_UDS);
	}
	omSettingsWnd->ShowWindow(SW_SHOW);
	objParent.Detach();
	return 0;
}

//________________________________________________________________________________________________________________________________________________________________
//________________________________________________________________________________________________________________________________________________________________

USAGEMODE HRESULT TX_vSetDILInterfacePtrUDS(void* ptrDILIntrf)
{
	CUDSMainWnd::s_podGetUdsMsgManager()->vSetDILInterfacePtr(ptrDILIntrf);
	//    m_spodInstance = CUDSMainWnd::s_podGetUdsMsgManager();//->vSetDILInterfacePtr(ptrDILIntrf);
	return S_OK;
}

/**********************************************************************************************************
 Function Name  :   UpdateFields
 Description    :   This function is called from the SettingsWnd to save all the values tha were settled 
					in the settings Window. This function also sets some values in the MainWnd such as 
					SA, TA,CANId
 Member of      :   CUDS_Protocol
 Author(s)      :   S�nchez A
 Date Created   :   01.03.2013
**********************************************************************************************************/

void CUDS_Protocol::UpdateFields()
{
	if(omMainWnd!=NULL){
		omMainWnd->fInterface =fInterface;	
		switch (fInterface){ 
			case INTERFACE_NORMAL_11:
				{
					omMainWnd->CanID = MsgID;
					omMainWnd->m_omSourceAddress.vSetValue(SourceAddress);
					omMainWnd->m_omTargetAddress.vSetValue(TargetAddress);

					omMainWnd->m_omSourceAddress.SetReadOnly(TRUE);
					omMainWnd->m_omTargetAddress.SetReadOnly(TRUE);					
					omMainWnd->m_omCanID.vSetValue(MsgID);
				}
				break;
			case INTERFACE_EXTENDED_11:
				{
					omMainWnd->CanID =MsgID  /*MsgID2*/;  
					omMainWnd->m_omSourceAddress.SetReadOnly(FALSE);
					omMainWnd->m_omTargetAddress.SetReadOnly(FALSE);
					omMainWnd->m_omSourceAddress.SetLimitText(2);
					omMainWnd->m_omTargetAddress.SetLimitText(2);
					if(MsgID ==0){
						omMainWnd->m_omSourceAddress.vSetValue(0);
						omMainWnd->m_omTargetAddress.vSetValue(0);
					
					} 
					SourceAddress = omMainWnd->m_omSourceAddress.lGetValue();
					TargetAddress = omMainWnd->m_omTargetAddress.lGetValue(); 

					int MsgID2 =	SourceAddress + MsgID;
					omMainWnd->m_omCanID.vSetValue(MsgID2);
					respID = TargetAddress + RespMsgID;
				}
				break;
			case INTERFACE_NORMAL_ISO_29:
				{
					omMainWnd->CanID = MsgID;
					omMainWnd->m_omSourceAddress.SetLimitText(3);
					omMainWnd->m_omTargetAddress.SetLimitText(3);
					omMainWnd->m_omSourceAddress.vSetValue(SourceAddress);
					omMainWnd->m_omTargetAddress.vSetValue(TargetAddress);
					omMainWnd->m_omSourceAddress.SetReadOnly(false);
					omMainWnd->m_omTargetAddress.SetReadOnly(false);					
					omMainWnd->m_omCanID.vSetValue(MsgID);
				}
				break;
			case INTERFACE_NORMAL_J1939_29:
				{
					omMainWnd->CanID = MsgID;
					omMainWnd->m_omSourceAddress.SetLimitText(2);
					omMainWnd->m_omTargetAddress.SetLimitText(2);
					omMainWnd->m_omSourceAddress.vSetValue(SourceAddress);
					omMainWnd->m_omTargetAddress.vSetValue(TargetAddress);
					omMainWnd->m_omSourceAddress.SetReadOnly(false);
					omMainWnd->m_omTargetAddress.SetReadOnly(false);					
					omMainWnd->m_omCanID.vSetValue(MsgID);
				}
				break;		
		}
	}


}

/**********************************************************************************************************
 Function Name  :   StartTimer_Disable_Send
 Description    :   This function is called from the CUDS_NegRespMng to disable the send button and to
					start again the default timer					
 Member of      :   CUDS_Protocol
 Author(s)      :   S�nchez A
 Date Created   :   11.06.2013
**********************************************************************************************************/

void CUDS_Protocol::StartTimer_Disable_Send()
{
	omMainWnd->m_omSendButton.EnableWindow(FALSE);
	//omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);		     	 //ReStar the timer that Enables the SendButton
	omMainWnd->SetTimer(ID_TIMER_SEND_BUTTON,P2_Time , NULL);	// Time to wait to able the send button again
}
/**********************************************************************************************************
 Function Name  :   StartTimer_Disable_Send
 Description    :   This function is called from the CUDS_NegRespMng to able the SEND button and to
					fill the timer					
 Member of      :   CUDS_Protocol
 Author(s)      :   S�nchez A
 Date Created   :   11.06.2013
**********************************************************************************************************/

void CUDS_Protocol::KillTimer_Able_Send()
{
	omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);			 //ReStar the timer that Enables the SendButton
	omMainWnd->m_omSendButton.EnableWindow(TRUE);
	//omMainWnd->SetTimer(ID_TIMER_SEND_BUTTON,P2_Time	 /*TIME_UNABLE_SEND_BUTTON*/, NULL);	// Time to wait to able the send button again
}
//________________________________________________________________________________________________________________________________________________________________
//________________________________________________________________________________________________________________________________________________________________


