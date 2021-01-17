﻿// lab1_2Client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "lab1_2Client.h"
#include "Message.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Единственный объект приложения

CWinApp theApp;

int ClientId;
mutex hMutex;
bool connection;

void canalStart(CSocket& hS) {
    hS.Create();
    hS.Connect(_T("127.0.0.1"), 11111);
}

void canalStop(CSocket& hS) {
    hS.Close();
}

void getData() {
    while (true) {
        CSocket client;
        canalStart(client);
        Message m;
        Message::SendMessage(client, 0, ClientId, M_GETDATA);
        MsgHeader h_message;
        h_message = m.Receive(client);
        canalStop(client);
        if (h_message.m_Type == M_TEXT) {
            hMutex.lock();
            cout << "Сообщение от клиента"<<m.getM_Header().m_From<<": "<<m.getM_Data() << endl;
            hMutex.unlock();
        }

            
        Sleep(1000);
    }
}

void ConnectToServer(Message& m, MsgHeader& h_message, CSocket& client) {

    AfxSocketInit();
    canalStart(client);
    Message::SendMessage(client, 0, 0, M_INIT);
    h_message = m.Receive(client);
    canalStop(client);

    if (h_message.m_Type == M_CONFIRM) {
        ClientId = h_message.m_To;
        hMutex.lock();
        cout << "Ваш ID " << ClientId << endl;
        hMutex.unlock();
        thread t(getData);
        t.detach();
        
    }
    else {
        cout << "Ошибка. Клиент не подключен." << endl;
        return;
    }
}

void start() {
    cout << "Выберите: 1 - подключиться к серверу. \n 0 - выйти." << endl;
    int answer;
    cin >> answer;
       if (answer == 0)
        return;
    MsgHeader h_message;
    CSocket client;
    Message m;
    ConnectToServer(m, h_message, client);
        while (true) {
            
            cout << "Выберите: 0 - отправить сообщение. \n 1 - выйти." << endl;
            cin >> answer;

            switch (answer)
            {
            case 0: {
                
                    string str;
                    int ID;
                    cout << "Выберите: 0 - одному клиенту.  \n1 - всем клиентам." << endl;
                    int choice;
                    cin >> choice;
                    switch (choice) {
                    case 0: {
                        cout << "Введите ID клиента." << endl;
                        
                        cin >> ID;
                        break;
                    }
                    case 1: {
                        ID = A_ALL;
                        break;
                    }
                    }


                    
                    cout << "Введите сообщение" << endl;
                    cin.ignore();
                    getline(cin, str);
                    canalStart(client);
                   
                    Message::SendMessage(client, ID, ClientId, M_TEXT, str);
                    
                    h_message = m.Receive(client);
                    canalStop(client);
                    hMutex.lock();
                    if (h_message.m_Type == M_CONFIRM) {
                        cout << "Сообщение отправлено." << endl;
                    }
                    else {
                        cout << "Ошибка. Сообщение не отправлено." << endl;
                    }
                    hMutex.unlock();
                    break;
                }
               
                
            

            case 1: {
               
                    canalStart(client);
                    Message::SendMessage(client, 0, ClientId, M_EXIT0);
                    h_message = m.Receive(client);
                    hMutex.lock();
                    if (h_message.m_Type == M_CONFIRM)
                    {
                        cout << "Успешно." << endl;
                        hMutex.unlock();
                        connection = false;
                        return;
                    }
                    else {
                        cout << "Ошибка." << endl;
                        hMutex.unlock();
                    }
                    
                
                break;
            }
            default:
                cout << "Выберите 0 или 1." << endl;
                break;
            }
        }
    
}

int main()
{
    setlocale(LC_ALL, "rus");
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            HWND hwnd = GetConsoleWindow();
            HMENU hmenu = GetSystemMenu(hwnd, FALSE);
            EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
            start();
            // TODO: вставьте сюда код для приложения.
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
