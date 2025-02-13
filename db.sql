create table skin (
    skin_id BIGSERIAL NOT NULL PRIMARY KEY,
    skin VARCHAR(50) NOT NULL,
    price BIGINT CHECK (price > 0)
);

create table client (
    client_uid UUID NOT NULL PRIMARY KEY,
    nickname VARCHAR(50) NOT NULL,
    email VARCHAR(50) NOT NULL, 
    pwd VARCHAR(50) NOT NULL,
    gold BIGINT NOT NULL,
    skins integer[],
    UNIQUE(email),
    UNIQUE(nickname)
);

create table message (
    message_uid UUID NOT NULL PRIMARY KEY,
    sender UUID REFERENCES client(client_uid),
    receiver UUID REFERENCES client(client_uid),
    msg VARCHAR(50) NOT NULL    
);


create table game (
    game_uid UUID NOT NULL PRIMARY KEY,
    winner UUID REFERENCES client(client_uid),
    players UUID[]
);

insert into skin (skin, price) values ('pes patron', '999');