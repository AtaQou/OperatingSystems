#!/bin/bash

# Αντωνίου Σωτήριος, 1067512 
# Κωνσταντίνος Χριστάκος, 1070903 

# Ορισμός του αρχείου δεδομένων με τα επιβάτες
DATA_FILE="./passenger_data.csv"

# Συνάρτηση για αναζήτηση και εμφάνιση στοιχείων επιβάτη
search_passenger() {
    if [[ -z "$1" ]]; then
        echo "Παρακαλώ εισάγετε το όνομα ή το επώνυμο του επιβάτη."
        return
    fi

    name="$1"
    results=$(grep -i "$name" "$DATA_FILE") # Αναζήτηση με βάση το όνομα (case-insensitive)

    if [[ -z "$results" ]]; then
        echo "Δεν βρέθηκαν εγγραφές για τον επιβάτη: $name"
        return
    fi

    echo "Στοιχεία επιβάτη:"
    echo "$results" | while IFS=";" read -r code fullname age country status rescued; do
        echo "Κωδικός: $code"
        echo "Όνομα: $fullname"
        echo "Ηλικία: $age"
        echo "Χώρα: $country"
        echo "Κατηγορία: $status"
        echo "Διασωθείς: $rescued"
        echo "---------------------------"
    done
}

# Συνάρτηση για εισαγωγή δεδομένων
insert_data() {
    if [[ -z "$1" ]]; then
        echo "Δεν δόθηκε όνομα αρχείου. Εισάγετε δεδομένα χειροκίνητα."
        while : ; do
            read -p "Εισάγετε τα δεδομένα του επιβάτη (κωδικός, όνομα, ηλικία, χώρα, κατάσταση, διασωθείς): " line
            if [[ -z "$line" ]]; then
                break
            fi
            echo "$line" >> "$DATA_FILE"
        done
    else
        echo "Αρχείο: $1"
        cp "$1" "$DATA_FILE"
    fi
}

# Συνάρτηση για αλλαγή στοιχείων επιβάτη
update_passenger() {
    if [[ $# -lt 2 ]]; then
        echo "Λάθος αριθμός ορισμάτων. Η εντολή πρέπει να έχει τη μορφή: ./processes_ipc.sh update <κωδικός ή το όνομα ή το επώνυμο του επιβάτη> <πεδίο>:<νέα τιμή>"
        return
    fi

    search_key=$1
    field_value=$2

    field=$(echo "$field_value" | cut -d: -f1)
    new_value=$(echo "$field_value" | cut -d: -f2-)

    if [[ "$field" == "record" ]]; then
        record=$(grep -i "$search_key" "$DATA_FILE")

        if [[ -z "$record" ]]; then
            echo "Δεν βρέθηκαν δεδομένα για αυτόν τον επιβάτη."
            return
        fi

        if [[ "$record" == "$new_value" ]]; then
            echo "Η εγγραφή είναι ήδη η επιθυμητή και δεν χρειάζεται να γίνει αλλαγή."
            return
        fi

        sed -i "s|^$record\$|$new_value|" "$DATA_FILE"
        echo  "Η εγγραφή ενημερώθηκε από:$record σε: $new_value"
    else
        record=$(grep -i "$search_key" "$DATA_FILE")

        if [[ -z "$record" ]]; then
            echo "Δεν βρέθηκαν δεδομένα για αυτόν τον επιβάτη."
            return
        fi

        updated_record=$(echo "$record" | awk -F';' -v field="$field" -v new_value="$new_value" 'BEGIN {OFS=FS}
            {
                if (field == "fullname") $2 = new_value;
                else if (field == "age") $3 = new_value;
                else if (field == "country") $4 = new_value;
                else if (field == "status") $5 = new_value;
                else if (field == "rescued") $6 = new_value;
                print $0;
            }')
        
        sed -i "s|^$record\$|$updated_record|" "$DATA_FILE"
        echo "Η εγγραφή ενημερώθηκε από: $record σε: $updated_record"
    fi
}

# Συνάρτηση για προβολή όλων των επιβατών
display_file() {
    lines_per_page=$(tput lines)  # Παίρνει το ύψος της τερματικής οθόνης
    ((lines_per_page -= 2))       # Αφαιρούμε γραμμές για prompt και περιθώριο

    if [[ ! -f "$DATA_FILE" ]]; then
        echo "Το αρχείο $DATA_FILE δεν βρέθηκε."
        return
    fi

    total_lines=$(wc -l < "$DATA_FILE")  # Συνολικές γραμμές στο αρχείο
    start_line=1

    while ((start_line <= total_lines)); do
        sed -n "${start_line},$((start_line + lines_per_page - 1))p" "$DATA_FILE"
        read -p "Πατήστε <space> για συνέχεια ή <q> για έξοδο..." -n 1 key
        echo
        if [[ $key == "q" ]]; then
            echo "Η προβολή τερματίστηκε."
            break
        fi
        start_line=$((start_line + lines_per_page))
    done
}

# Συνάρτηση για δημιουργία αναφορών
generate_reports() {
    # 1. Εύρεση και προβολή ηλικιακών ομάδων
    echo "Ηλικιακές ομάδες:" > ages.txt
    
    # Χρήση awk για να εντοπιστούν οι ηλικιακές ομάδες, ταξινόμηση και αφαίρεση διπλότυπων
    awk -F";" '{age=$3; 
        if (age <= 18) print "0-18"; 
        else if (age <= 35) print "19-35"; 
        else if (age <= 50) print "36-50"; 
        else print "51+"}' "$DATA_FILE" | sort | uniq -c > ages.txt

    # 2. Υπολογισμός ποσοστού διασωθέντων ανά ηλικιακή ομάδα
    echo -e "Αναφορά ποσοστών διασωθέντων ανά ηλικιακή ομάδα:\n" > percentages.txt

    # Υπολογισμός counters από το αρχείο δεδομένων για κάθε ηλικιακή ομάδα
    awk -F";" '{
        age = $3;

        if (age == "") next;  # Παράλειψη αν η ηλικία είναι κενή

        if (age <= 18) {
            counter18++;
        } else if (age <= 35) {
            counter35++;
        } else if (age <= 50) {
            counter50++;
        } else {
            counter51++;
        }

        total_all++;  # Αύξηση του συνολικού αριθμού επιβατών
    } END {
        printf "counter18=%d\ncounter35=%d\ncounter50=%d\ncounter51=%d\ntotal_all=%d\n", counter18, counter35, counter50, counter51, total_all;
    }' "$DATA_FILE" > tmp_counters.sh

    # Εκτέλεση και φόρτωση των τιμών των counters
    source tmp_counters.sh
    rm tmp_counters.sh

    # Υπολογισμός counters_rescued για τα "yes" στην κατάσταση διάσωσης
    grep -i "yes" "$DATA_FILE" | awk -F";" '{
        age = $3;

        if (age == "") next;  # Παράλειψη αν η ηλικία είναι κενή

        if (age <= 18) {
            counter_rescued18++;
        } else if (age <= 35) {
            counter_rescued35++;
        } else if (age <= 50) {
            counter_rescued50++;
        } else {
            counter_rescued51++;
        }

        total_rescued++;  # Αύξηση του συνολικού αριθμού διασωθέντων
    } END {
        printf "counter_rescued18=%d\ncounter_rescued35=%d\ncounter_rescued50=%d\ncounter_rescued51=%d\ntotal_rescued=%d\n", counter_rescued18, counter_rescued35, counter_rescued50, counter_rescued51, total_rescued;
    }' > tmp_counters_rescued.sh

    # Εκτέλεση και φόρτωση των τιμών των counters_rescued
    source tmp_counters_rescued.sh
    rm tmp_counters_rescued.sh

    # Υπολογισμός ποσοστού διασωθέντων με χρήση των μεταβλητών counters
    awk -v counter18="$counter18" -v counter_rescued18="$counter_rescued18" \
        -v counter35="$counter35" -v counter_rescued35="$counter_rescued35" \
        -v counter50="$counter50" -v counter_rescued50="$counter_rescued50" \
        -v counter51="$counter51" -v counter_rescued51="$counter_rescued51" \
        -v total_all="$total_all" -v total_rescued="$total_rescued" 'BEGIN {
        printf "%-15s %-20s %-15s %s\n", "Ηλικιακή Ομάδα", "Συνολικοί Επιβάτες", "Διασωθέντες", "Ποσοστό (%)" >> "percentages.txt"
        print "-----------------------------------------------------------" >> "percentages.txt";

        # Υπολογισμός ποσοστών για κάθε ηλικιακή ομάδα
        percentage18 = (counter18 > 0) ? (counter_rescued18 / counter18) * 100 : 0;
        printf "%-15s %-20d %-15d %.2f%%\n", "0-18", counter18, counter_rescued18, percentage18 >> "percentages.txt";

        percentage35 = (counter35 > 0) ? (counter_rescued35 / counter35) * 100 : 0;
        printf "%-15s %-20d %-15d %.2f%%\n", "19-35", counter35, counter_rescued35, percentage35 >> "percentages.txt";

        percentage50 = (counter50 > 0) ? (counter_rescued50 / counter50) * 100 : 0;
        printf "%-15s %-20d %-15d %.2f%%\n", "36-50", counter50, counter_rescued50, percentage50 >> "percentages.txt";

        percentage51 = (counter51 > 0) ? (counter_rescued51 / counter51) * 100 : 0;
        printf "%-15s %-20d %-15d %.2f%%\n", "51+", counter51, counter_rescued51, percentage51 >> "percentages.txt";

        # Υπολογισμός συνολικού ποσοστού
        total_percentage = (total_all > 0) ? (total_rescued / total_all) * 100 : 0;
        print "-----------------------------------------------------------" >> "percentages.txt";
        printf "%-15s %-20d %-15d %.2f%%\n", "ΣΥΝΟΛΟ", total_all, total_rescued, total_percentage >> "percentages.txt";

        print "-----------------------------------------------------------" >> "percentages.txt";
    }'

    # 3. Υπολογισμός μέσης ηλικίας ανά κατηγορία επιβατών
    echo "Μέση ηλικία ανά κατηγορία επιβατών:" > avg.txt
    awk -F";" '{
        status=$5; age=$3
        if (status == "Passenger" || status == "Crew" || status == "VIP") {
            sum[status]+=$3
            count[status]++
        }
    } END {
        for (i in sum) {
            if (count[i] > 0)
                print i, sum[i]/count[i]
        }
    }' "$DATA_FILE" > avg.txt
        
    # 4. Φιλτράρισμα διασωθέντων
    echo "Διασωθέντες:" > rescued.txt
    grep -i "yes" "$DATA_FILE" > rescued.txt
}

# Λειτουργία επιλογών
if [[ $# -eq 0 ]]; then
    echo "Χρησιμοποιήστε: ./processes_ipc.sh [insert|search|update|display|reports]"
    exit 1
fi

case $1 in
    insert)
        insert_data "$2"
        ;;
    search)
        search_passenger "$2"
        ;;
    update)
        update_passenger "$2" "$3"
        ;;
    display)
        display_file
        ;;
    reports)
        generate_reports
        ;;
    *)
        echo "Ακατανόητη εντολή. Χρησιμοποιήστε: ./processes_ipc.sh [insert|search|update|display|reports]"
        ;;
esac