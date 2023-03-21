#### Course: Network programming in C++  
#### Lesson 08. Multi-agent systems and peer-to-peer networks.

***  

#### Задачи.  

  1. Переделайте <b>torrent</b>-клиент так, чтобы он загружал и по Magnet-ссылкам, и по torrent-файлам.  
  2. Реализуйте простейший <b>P2P-мессенджер</b>.  

***  

#### Комментарии.  

-  <b>Local P2P Chat</b>.  
   This is a fully decentralized chat. To communicate, simply run it on computers in a single local network (using one port). All messages are encoded in UTF-16, so national characters are supported, the only restriction is the font of the console.  

   You can use `-p` or `-port` argument to select the port you want to use (The default port is 8001)
   ``` bash
   ./chat.exe -port 1337
   ```  

   <b>How it works.</b>  
   It is based on UDP datagrams that are sent by a broadcast request to the network and are not encrypted in any way. If you wish, you can write your client that will work correctly with mine or just view messages using wireshark or something like it. The structure of all data sent is as follows (Yes, this is JSON):  
   ```json
    {"type": "", "data": [{"":""}] }
   ```  
   For example, this is information about sending a message:  
   ```json
    {"type": "message", 
       "data": [
           { "user": "2sha" },
           { "content": "Hello, everybody!" }
       ]}
   ```  
   There are 5 types of data:  
    * `message` - the user sent a message  
    * `user` - name of the user (wow)  
    * `content` - the content of the message (so unexpectedly)  
    * `add_user` - someone's joined to our chat  
    * `user` - stranger's name  
    * `remove_user` - someone left our chat  
    * `user` - name of miserable  
    * `user_list_req` - someone wants to know the list of users on the network. We are obliged to respond to this message  
    * `user_list_res` - response to `user_list_req` request  
    * `user` - our username  
