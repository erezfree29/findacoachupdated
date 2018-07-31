class AddmoretoTrainerAdress < ActiveRecord::Migration[5.2]
  def change
    add_column :trainers, :address2, :string
    add_column :trainers, :address3, :string

  end
end
