import random

def generate_db(n=5000):
    categorie = {
        "Illuminazione": 10, "Rifiuti": 20, "Strade": 30, "Verde Pubblico": 40, 
        "Incendio": 50, "Allagamento": 60, "Segnaletica": 70, "Edilizia": 80, 
        "Randagismo": 90, "Inquinamento": 11, "Sicurezza": 21
        }
    
    nomi = ["Mario", "Luca", "Giuseppe", "Anna", "Paola", "Salvatore", "Elena", "Maria",
            "Andrea", "Rosa", "Angela", "Francesco", "Alessandro", "Roberto", "Stefano",
            "Giovanni", "Marco", "Filippo", "Chiara", "Giulia", "Francesca", "Silvia", 
            "Federica", "Lorenzo", "Simone", "Matteo", "Davide", "Antonio", "Michele", 
            "Vincenzo", "Emanuele", "Caterina", "Luisa", "Sofia", "Alice", "Valentina"
            ]
    cognomi = ["Rossi", "Bianchi", "Pisu", "Esposito", "Rizzo", "Ferrari", "Marino", "Greco", 
               "Bruno", "Gallo", "Conti", "De Luca", "Costa", "Giordano", "Mancini",    
               "Russo","Lombardi", "Barbieri", "Fontana", "Santoro", "Mariani", "Ferrara", 
               "Serra", "Motta", "Pellegrini", "Villa", "Farina"
               ]
    
    with open("database.txt", "w") as f:
        for _ in range(1, n + 1):
            # Scelta categoria e prefisso
            nome_cat, prefisso = random.choice(list(categorie.items()))
            
            # Creazione ID: Prefisso + 4 cifre casuali (es: 50 + 1234 = 501234)
            # Usiamo zfill(4) per garantire che siano sempre 4 cifre dopo il prefisso
            id_corpo = random.randint(1, 9999)
            id_segnalazione = f"{prefisso}{id_corpo:04d}" 
            
            cittadino = f"{random.choice(nomi)} {random.choice(cognomi)}"
            desc = f"Problema segnalato in via {random.choice(cognomi)} numero {random.randint(1, 150)}"
            # Data normalizzata YYYYMMDD
            data = 20260000 + (random.randint(1, 12) * 100) + random.randint(1, 28)
            urgenza = random.randint(1, 5)
            stato = random.randint(0, 2) # Solo 0, 1, 2 (Aperto, Risoluzione, Chiuso)
            
            # Scrittura nel formato richiesto: 7 righe per blocco
            f.write(f"{id_segnalazione}\n{cittadino}\n{nome_cat}\n{desc}\n{data}\n{urgenza}\n{stato}\n")

    print(f"Generato database.txt con {n} segnalazioni (Stile SSNNNN).")

if __name__ == "__main__":
    generate_db(5000)
