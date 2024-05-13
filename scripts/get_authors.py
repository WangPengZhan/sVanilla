import csv
import git
import argparse
import os
import shutil

def get_authors_and_emails():
    authors_emails = set()

    if not shutil.which("git"):
        authors_emails.add(("iveswang","1660583890@qq.com"))
        authors_emails.add(("AlanusMeminius","nononevent@outlook.com"))
    else:
        repo = git.Repo(os.getcwd())
        commits = repo.iter_commits('--all')

        for commit in commits:
            author_name = commit.author.name
            author_email = commit.author.email
            authors_emails.add((author_name, author_email))

    return authors_emails

def write_to_csv(authors_emails, csv_filename):
    with open(csv_filename, 'w', newline='', encoding='utf-8') as csvfile:
        csv_writer = csv.writer(csvfile)
        for author_email in authors_emails:
            csv_writer.writerow(author_email)

def main():
    parser = argparse.ArgumentParser(description='Get authors and emails')
    parser.add_argument('csv_filename', help='Path to the CSV file to write')

    args = parser.parse_args()

    authors_emails = get_authors_and_emails()
    write_to_csv(authors_emails, args.csv_filename)

if __name__ == "__main__":
    main()
