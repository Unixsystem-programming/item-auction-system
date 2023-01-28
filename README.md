# Unix system programing 설계 과제

* 주제 : 채팅기반 아이템 경매 시스템 

* 선정 이유 : 프로그램 제시 조건인 입출력함수 사용, 3개 이상 멀티쓰레드 동기화 상태, 파일시스템과 네트워크 사용을 모두 충족할 수 있는 프로그램이 무엇이 있을까 고민을 하던 중 여러 client를 멀티 스레드 기반 서버로 처리하는 채팅 프로그램이 적격이라는 생각을 하게 되었습니다. 
 <br> 그러나 일반 채팅프로그램은 평범하고 흔하기 때문에 프로그램의 방향성에 대해 다시 생각해 보았습니다. 프로그램 사용목적에 단순한 메시지 송신/수신 뿐 아니라, 특별한 무언가가 있으면 하였고 생각해본 컨셉은 온라인 경매 프로그램 입니다. 

* 아이디어 : 당근과 같은 온라인 중고 거래는 판매자에게 먼저 채팅을 시도하고 입금을 한 사람이 물건을 구매할 수 있습니다. 기존 선착순 시스템이 아닌 물건에 관심 있는 모든 사람들이구매를 시도할 수 있도록 경매를 통해 동등한 기회를 주는 방식의 프로그램을 고안하게 되었습니다.


## 프로그램 설명
- 멀티스레드 서버에  1명의 호스트와  3명이상의 client를 받는다.(소켓 프로그래밍)
- 제한된 시간동안 경매를 진행한다.(signal처리) 

<br> 1. 경매는 미리 지정된 값부터 시작하며, 호스트가 아이템에 관해 설명하고 경매를 시작한다. 
* 아이템의 가치에 따라 가격이 다르다.
* 아이템은 파일 형식이며, 파일 내용에서 낙찰된 아이템을 가져온다.

<br> 2. client들은 자신의 닉네임을 혹은 정해 놓은 키워드를 치면 값을 부를 수 있는 기회를 얻는다. 
* 직전에 불린 아이템 값보다 높아야 하며, 값은 계속 갱신되어야 한다.(동기화 필요)

<br> 3. 상황은 서버 - 호스트의 소통을 통해 client들에게 마지막 낙찰가가 누구인지 지속적으로 실시간으로 생중계 된다.

<br> 4. 일정시간동안 client들의 값 제시 요청이 없으면 경매 종료된다. 제일 높은 값을 부른 client가 낙찰 받는다. 호스트가 마무리 멘트와 경매를 종료한다.
* 경매 라운드 마다 아이템은 1개로 제한하고, 다음 경매는 다시 1번 부터 반복된다.
* ‘item.txt’ 파일안에 해당 아이템이 있다.

<br> 5. 경매 종료 후, 해당 아이템은 낙찰된 client에게 파일로 전송된다. (파일 시스템)

## 개발환경
Linux(Ubuntu)
## 파일 구조
## 실해방법
