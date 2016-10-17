# ifun_test
예전에 작업했던 내용을 백업.. 실제 프로젝트에는 반영되지 않음..

# 실행을 위해서 필요한 작업
1. MANIFEST.json 을 수정하고 해당하는 필요한 데이터 베이스와 zookeeper 서버가 필요하다.
  1. Object
    - 오브젝트로 선언된 데이터베이스가 저장 될 곳으로 주소와 아이디, 비밀번호, 데이터베이스 이름 등을 맞춰주어야한다.
  1. ResourceManager
    - MANIFEST.json과 ifun_test_game_data_objects.h를 참고하여 필요한 정보가 저장된 mysql 데이터베이스를 미리 준비해야한다.
  1. ZookeeperClient
    - 별도의 주키퍼 서버를 두지 않고 서버와 동일한곳에 주키퍼를 동작시킨다면 별도로 설정해주지 않아도 된다.
    - 즉, 변경하지 않고 작업하기 위해선 주키퍼를 구동시켜야한다.
    
# 구현내
 - 가입: join
 - 로그인: login
 - 로그아웃: logout
 - 덱 저장: save_deck
 - 덱 삭제: delete_deck
 - 던전 클리어: clear_dungeon
 - 상품 구매: buy_goods
