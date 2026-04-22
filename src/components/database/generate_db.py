import struct
import random

def generate_db(n=5000):
    # Formato C: i (int 4b), 64s (char[64]), 64s (char[64]), 1024s (char[1024]), i, i, i
    # Escludiamo il puntatore *next che verrà gestito in RAM dal C
    fmt = "<i64s64s1024siii"
    
    categorie = {
        "Illuminazione": 10, "Rifiuti": 20, "Strade": 30, "Verde Pubblico": 40, 
        "Incendio": 50, "Allagamento": 60, "Segnaletica": 70, "Edilizia": 80, 
        "Randagismo": 90, "Inquinamento": 11, "Sicurezza": 21
    }
    
    nomi = [
        "Mario", "Luca", "Giuseppe", "Anna", "Paola", "Salvatore", "Elena", "Maria",
        "Andrea", "Rosa", "Angela", "Francesco", "Alessandro", "Roberto", "Stefano",
        "Giovanni", "Marco", "Filippo", "Chiara", "Giulia", "Francesca", "Silvia", 
        "Federica", "Lorenzo", "Simone", "Matteo", "Davide", "Antonio", "Michele", 
        "Vincenzo", "Emanuele", "Caterina", "Luisa", "Sofia", "Alice", "Valentina"
    ]
    cognomi = [
        "Rossi", "Bianchi", "Pisu", "Esposito", "Rizzo", "Ferrari", "Marino", "Greco", 
        "Bruno", "Gallo", "Conti", "De Luca", "Costa", "Giordano", "Mancini",    
        "Russo","Lombardi", "Barbieri", "Fontana", "Santoro", "Mariani", "Ferrara", 
        "Serra", "Motta", "Pellegrini", "Villa", "Farina"
    ]
    
    with open("database.bin", "wb") as f:
        for _ in range(n):
            nome_cat, prefisso = random.choice(list(categorie.items()))
            
            # ID: Prefisso + 4 cifre (SSNNNN) come intero [cite: 91]
            id_val = int(f"{prefisso}{random.randint(1, 9999):04d}")
            
            cittadino = f"{random.choice(nomi)} {random.choice(cognomi)}"
            via = random.choice(cognomi)
            desc = f"Problema in via {via} civico {random.randint(1, 150)}"
            
            # Tua Data Mostro YYYYMMDD
            data = 20260000 + (random.randint(1, 12) * 100) + random.randint(1, 28)
            urgenza = random.randint(1, 5)
            stato = random.randint(0, 2)
            
            # Pack binario: convertiamo le stringhe in bytes
            record = struct.pack(fmt, 
                id_val,
                cittadino.encode('utf-8')[:63],
                nome_cat.encode('utf-8')[:63],
                desc.encode('utf-8')[:1023],
                data,
                urgenza,
                stato
            )
            f.write(record)

    print(f"Generato database.bin con {n} segnalazioni.")

if __name__ == "__main__":
    generate_db(25000)
