double joint[3];

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13,LOW);
}

void loop() {
  if(Serial.available())
  {
    String inputStr = Serial.readStringUntil('\n');
    //Serial.println(inputStr);
    Split(inputStr,',');
    if (joint[2] == 104.20192){
      digitalWrite(13, HIGH);
      Serial.print(joint[0],13);
    }else{
      digitalWrite(13, LOW);
    };
  }
}

void Split(String sData, char cSeparator)
{	
	int nCount = 0;
	int nGetIndex = 0 ;
 
	//임시저장
	String sTemp = "";
 
	//원본 복사
	String sCopy = sData;
  int i = 0;
	while(true)
	{
		//구분자 찾기
		nGetIndex = sCopy.indexOf(cSeparator);
 
		//리턴된 인덱스가 있나?
		if(-1 != nGetIndex)
		{
			//있다.
 
			//데이터 넣고
			sTemp = sCopy.substring(0, nGetIndex);
      joint[i] = sTemp.toFloat();
			Serial.println( sTemp );
		
			//뺀 데이터 만큼 잘라낸다.
			sCopy = sCopy.substring(nGetIndex + 1);
		}
		else
		{
			//없으면 마무리 한다.
			Serial.println( sCopy );
      joint[i] = sCopy.toFloat();
			break;
		}
 
		//다음 문자로~
		++nCount;
    ++i;
	}
 
}