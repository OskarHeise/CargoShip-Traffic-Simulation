# Operating Systems Project 22/23

## [Project Brief 05/11/2022](https://informatica.i-learn.unito.it/enrol/index.php?id=2548)

## Project Description

## 1. Student Group Composition
The project is to be developed by a group consisting of 1 to a maximum of 3 members. The group should be composed of students from the same shift who will discuss with the teacher of their shift. However, discussion of the laboratory project by a group of students from different shifts is also allowed. In this case, all students in the group will discuss with the same teacher. Example: Tizio (shift T1) and Caio (shift T2) decide to do the project together. They submit it and are summoned by Prof. Sempronio on day X. On that day, Tizio and Caio both show up and receive evaluations from Prof. Sempronio (teacher of T1, even though Caio is part of shift T2, whose reference teacher is Prof. Calpurnio).

## 2. Submission
The project consists of:
1. Source code.
2. A brief report summarizing the design choices made.
The project is submitted by filling out the following Google Form, accessed with institutional credentials:
- Google Form Link
This form will require the upload of:
- The project itself (a single file in .tgz or .zip format, NOT .rar).
- Last name, first name, ID, and email of each group member.
After uploading the project, you will be summoned by the teacher for discussion (see Section 1 in case of a group composed of students from different shifts). Note: Fill out only one form per project (not one for each group member). Any additional submission before the appointment will cancel the appointment date.
The submission must be made at least 10 days before the written exams to allow the teacher to schedule discussions:
- If submitted at least 10 days before an exam date, the teacher will propose a discussion date within the following exam.
- Otherwise, the date will be after the following exam.
Example: To ensure an appointment for project discussion within the exam on 24/01/2023, submit by 11:59 PM on 14/01/2023.

## 3. Evaluation and Validity
The project described in this document can be discussed if submitted by November 30, 2023. Starting from December 2023, the project assigned during the academic year 2023/24 will be discussed.
All group members must participate in the discussion. The project evaluation is individual and expressed in thirtieths. During the discussion:
- You will be asked to explain the project.
- Questions related to the "Unix" course program may be asked, not necessarily linked to the project development.
You need to achieve a minimum score of 18 out of 30 to be admitted to the written exam. If you pass the project discussion, the score obtained will allow you to participate in the written exam for the five subsequent exams after the passing date. There are no exceptions or waivers to this validity period.
In case of failure, the student can only reappear at least one month after the failed date.
Please note that the project score counts for 1/4 of the final Operating Systems course grade.

## 4. Project Publication, Plagiarism, Sanctions
Copying other projects or parts of them hinders proper evaluation. Therefore, students submitting the project are aware that:
- Sharing with other groups through public or private channels (e.g., Google Drive, Telegram channels, GitHub, etc.) of all or part of the project is not allowed until November 2023.
- Copying all or part of the project is not allowed.
- Any code fragments extracted from lecture code or other public repositories must be properly declared.
If a student fails to comply with the above-mentioned rules of conduct, after being summoned and given the opportunity to explain their position, they may face the following sanctions:
- If the student has passed the Operating Systems exam in the meantime, even after the project discussion date, the verbalization of the grade will be annulled.
- If the student has only passed the project discussion but not the exam, the project evaluation will be annulled, and the student cannot access further project discussions until the two subsequent exams after the plagiarism date.

## 5. Project Description: Minimal Version (Max 24 out of 30)
The goal is to simulate the traffic of cargo ships for the transportation of various goods through ports. This is achieved through the following processes:
- A master process responsible for creating other processes and managing the simulation.
- SO_NAVI (≥ 1) ship processes.
- SO_PORTI (≥ 4) port processes.
In the project description, we will refer to:
- Simulated time: the time passing in the simulation (e.g., one day to transport a cargo).
- Real-time: the duration of the simulation execution (e.g., after starting, the simulation ends after 30 seconds, although it simulated a duration of 30 days).
In the simulation, one day of simulated time equals one second of real-time.

### 5.1 Goods
The simulation includes SO_MERCI different types of goods. If deemed useful, each type of goods can be identified by a name or numerical identifier. Each lot of goods is characterized by:
- A quantity of goods (in tons), randomly extracted between 1 and SO_SIZE at the beginning of the simulation.
- A lifespan (in days), randomly extracted between SO_MIN_VITA and SO_MAX_VITA, measured in days.
Supply and demand for goods are generated at ports (see description below). Every time a type of goods is generated at runtime, it will always have the same characteristics as mentioned above.
When the lifespan of goods expires, the goods are lost and disappear from wherever they are (ship or port). The lifespan of goods is relative to the moment of their creation at a certain port.
All goods generated during the simulation fall into these categories:
- Goods present at a port (available for loading)
- Goods on a ship
- Goods delivered to a requesting port
- Goods expired at a port
- Goods expired on a ship

### 5.2 The Map
The world map is represented by a square of side SO_LATO (double), measured in kilometers (Flat Earth). A position on

